module;

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

module vrock.pdf.PDFBaseObjects;

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
        : PageBaseObject( std::move( dict ), parent, std::move( ctx ) )
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
            content.emplace_back( con );
        else if ( auto arr = dictionary->get<PDFArray>( "Contents" ) )
            for ( int i = 0; i < arr->value.size( ); ++i )
                if ( auto ref = arr->get<PDFRef>( i ) )
                    if ( auto con = context->get_object<PDFStream>( ref ) )
                        content.emplace_back( con );
    }

    PageTreeNode::PageTreeNode( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> ctx,
                                PageTreeNode *parent )
        : PageBaseObject( std::move( dict ), parent, std::move( ctx ), false )
    {
        if ( auto c = dictionary->get<PDFInteger>( "Count" ) )
            count = c->value;

        if ( auto k = dictionary->get<PDFArray>( "Kids" ) )
        {
            for ( int i = 0; i < k->value.size( ); ++i )
            {
                if ( auto ref = k->get<PDFRef>( i ) )
                {
                    if ( auto kid = context->get_object<PDFDictionary>( ref ) )
                    {
                        if ( auto type = kid->get<PDFName>( "Type" ) )
                        {
                            if ( type->name == "Pages" )
                                kids.emplace_back( std::make_shared<PageTreeNode>( kid, context, this ) );
                            else if ( type->name == "Page" )
                                kids.emplace_back( std::make_shared<Page>( kid, context, this ) );
                        }
                    }
                }
            }
        }
    }
} // namespace vrock::pdf