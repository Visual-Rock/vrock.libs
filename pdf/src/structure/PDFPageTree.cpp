module;

import vrock.pdf.PDFBaseObjects;
import vrock.pdf.RenderableObject;
import vrock.utils.List;

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

module vrock.pdf.PDFPageTree;

namespace vrock::pdf
{
    auto PageBaseObject::get_property( const std::string &name ) -> std::shared_ptr<PDFBaseObject>
    {
        auto obj = dictionary->get( name );
        if ( obj != nullptr )
            return obj;
        if ( parent == nullptr )
            return nullptr;
        else
            return parent->get_property( name );
    }

    Page::Page( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> ctx, PageTreeNode *parent )
        : PageBaseObject( std::move( dict ), parent, ctx ), Renderable( ctx )
    {
        auto get_box = [ this ]( const std::string &name, std::shared_ptr<Rectangle> _default ) {
            if ( auto array = get_property<PDFArray>( name ) )
                return std::make_shared<Rectangle>( array );
            return _default;
        };
        media_box = get_box( "MediaBox", nullptr );
        crop_box = get_box( "CropBox", media_box );
        bleed_box = get_box( "BleedBox", crop_box );
        trim_box = get_box( "TrimBox", crop_box );
        art_box = get_box( "ArtBox", crop_box );

        if ( auto rot = get_property<PDFInteger>( "Rotate" ) )
            rotation = rot->value;

        if ( auto con = dictionary->get<PDFStream>( "Contents" ) )
            content_streams.emplace_back( con );
        else if ( auto arr = dictionary->get<PDFArray>( "Contents" ) )
            for ( int i = 0; i < arr->value.size( ); ++i )
                if ( auto ref = arr->get<PDFRef>( i ) )
                    if ( auto con = PageBaseObject::context->get_object<PDFStream>( ref ) )
                        content_streams.emplace_back( con );

        if ( auto res = get_property<PDFDictionary>( "Resources" ) )
            resources = std::make_shared<ResourceDictionary>( res, PageBaseObject::context );
    }

    PageTreeNode::PageTreeNode( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> context,
                                PageTreeNode *parent )
        : PageBaseObject( std::move( dict ), parent, std::move( context ), false )
    {
        if ( auto c = dictionary->get<PDFInteger>( "Count" ) )
            count = c->value;

        if ( auto k = dictionary->get<PDFArray>( "Kids" ) )
        {
            kids.resize( k->value.size( ) );
            for ( int i = 0; i < k->value.size( ); ++i )
            {
                if ( auto kid = k->get<PDFDictionary>( i ) )
                {
                    if ( auto type = kid->get<PDFName>( "Type" ) )
                    {
                        if ( type->name == "Pages" )
                            kids[ i ] = std::make_shared<PageTreeNode>( kid, context, this );
                        // else if ( type->name == "Page" )
                        //     kids.emplace_back( std::make_shared<Page>( kid, context, this ) );
                    }
                }
            }
        }
    }

    auto PageTreeNode::get_page( std::size_t idx ) -> std::shared_ptr<Page>
    {
        // load page
        for ( std::size_t i = 0; i < kids.size( ); i++ )
        {
            if ( kids[ i ] == nullptr && idx == 0 )
            {
                if ( auto k = dictionary->get<PDFArray>( "Kids" ) )
                {
                    if ( auto kid = k->get<PDFDictionary>( i ) )
                    {
                        if ( auto type = kid->get<PDFName>( "Type" ) )
                        {
                            if ( type->name == "Page" )
                            {
                                kids[ i ] = std::make_shared<Page>( kid, context, this );
                                return kids[ i ]->to<Page>( );
                            }
                        }
                    }
                }
            }
            if ( kids[ i ] == nullptr )
                --idx;

            if ( auto page = kids[ i ]->to<Page>( ) )
            {
                if ( idx == 0 )
                    return page;
                else
                    --idx;
            }

            if ( auto node = kids[ i ]->to<PageTreeNode>( ) )
            {
                if ( node->count <= idx )
                    idx -= node->count;
                else
                    return node->get_page( idx );
            }
        }
        return nullptr;
    }

    PDFPageTree::PDFPageTree( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> ctx )
        : context( std::move( ctx ) )
    {
        if ( auto c = dict->get<PDFInteger>( "Count" ) )
            pages_count = c->value;
        pages.resize( pages_count );
        page_tree = std::make_shared<PageTreeNode>( dict, context, nullptr );
    }

    auto PDFPageTree::get_page( std::size_t idx ) -> std::shared_ptr<Page>
    {
        if ( idx > pages_count )
            return nullptr;
        if ( pages[ idx ] )
            return pages[ idx ];
        auto p = page_tree->get_page( idx );
        pages[ idx ] = p;
        return p;
    }

    auto PDFPageTree::get_pages( ) -> utils::List<std::shared_ptr<Page>>
    {
        if ( !all_pages_parsed )
        {
            for ( int i = 0; i < pages_count; ++i )
                if ( pages[ i ] == nullptr )
                    get_page( i );
            all_pages_parsed = true;
        }
        return pages;
    }

    auto PDFPageTree::get_page_count( ) -> std::int32_t
    {
        return pages_count;
    }
} // namespace vrock::pdf