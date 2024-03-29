#include "vrock/pdf/structure/RenderableObject.hpp"

#include "vrock/pdf/parser/ContentStreamParser.hpp"

#include <iostream>

namespace vrock::pdf
{
    RenderableObject::RenderableObject( RenderableObjectType t, std::int64_t z ) : z_index( z ), type( t )
    {
    }

    Image::Image( ) : RenderableObject( RenderableObjectType::Image, 0 )
    {
    }

    Image::Image( std::int64_t z ) : RenderableObject( RenderableObjectType::Image, z )
    {
    }

    Text::Text( ) : RenderableObject( RenderableObjectType::Text, 0 )
    {
    }

    Text::Text( std::int64_t z ) : RenderableObject( RenderableObjectType::Text, z )
    {
    }

    ResourceDictionary::ResourceDictionary( std::shared_ptr<PDFDictionary> dict_, std::shared_ptr<PDFContext> ctx )
        : dict( std::move( dict_ ) ), context( std::move( ctx ) )
    {
        if ( auto ext_g_states = dict->get<PDFDictionary>( "ExtGState" ) )
            for ( auto [ k, v ] : ext_g_states->dict )
                if ( auto val = ext_g_states->get<PDFDictionary>( k->name ) ) // to avoid getting PDFRef
                    ext_g_state[ k->name ] = val;

        if ( auto xobject = dict->get<PDFDictionary>( "XObject" ) )
            load_xobject( xobject );

        if ( auto font = dict->get<PDFDictionary>( "Font" ) )
        {
            for ( auto [ k, v ] : font->dict )
            {
                auto val = font->get<PDFDictionary>( k->name );
                if ( val )
                { // to avoid getting PDFRef
                    fonts[ k->name ] = std::make_shared<Font>( val );
                }
            }
        }
    }

    auto ResourceDictionary::load_xobject( std::shared_ptr<PDFDictionary> xobjects ) -> void
    {
        if ( xobjects )
        {
            for ( auto [ k, v ] : xobjects->dict )
            {
                std::cout << k->name << ' ';
                if ( auto val = xobjects->get( k->name ) )
                {
                    std::string type;
                    if ( auto stm = val->to<PDFStream>( ) )
                        if ( auto t = stm->dict->get<PDFName>( "Subtype" ) )
                            type = t->name;
                    std::cout << type << std::endl;
                    switch ( type.size( ) )
                    {
                    case 4: {
                        forms[ k->name ] = std::make_shared<Form>( val->to<PDFStream>( ), context );
                        break;
                    }
                    case 0:
                    case 5: {
                        images[ k->name ] = std::make_shared<PDFImage>( val->to<PDFStream>( ) );
                    }
                    }
                }
            }
        }

        if ( !dict->has( "XObject" ) )
            dict->set( "XObject", xobjects );
    }

    template <>
    auto to_renderable_object_type<Image>( ) -> RenderableObjectType
    {
        return RenderableObjectType::Image;
    }

    template <>
    auto to_renderable_object_type<Text>( ) -> RenderableObjectType
    {
        return RenderableObjectType::Text;
    }

    Renderable::Renderable( std::shared_ptr<PDFContext> ctx ) : context( std::move( ctx ) )
    {
    }

    auto Renderable::get_images( ) -> utils::List<std::shared_ptr<Image>>
    {
        parse_content( );
        return images;
    }

    auto Renderable::get_text( ) -> utils::List<std::shared_ptr<Text>>
    {
        parse_content( );
        return text;
    }

    auto Renderable::parse_content( ) -> void
    {
        if ( parsed )
            return;
        if ( content_streams.empty( ) )
        {
            parsed = true;
            return;
        }
        std::string content;
        for ( auto &content_stream : content_streams )
            content += content_stream->data;

        auto parser = ContentStreamParser( content, resources, context );
        images = std::move( parser.images );
        text = std::move( parser.text );
        parsed = true;
    }

    Form::Form( std::shared_ptr<PDFStream> form, std::shared_ptr<PDFContext> ctx ) : Renderable( std::move( ctx ) )
    {
        if ( auto array = form->dict->get<PDFArray>( "BBox" ) )
            bbox = std::make_shared<Rectangle>( array );
        content_streams.push_back( form );
        if ( auto res = form->dict->get<PDFDictionary>( "Resources" ) )
            resources = std::make_shared<ResourceDictionary>( res, context );
    }
} // namespace vrock::pdf