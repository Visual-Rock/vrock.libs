#include "vrock/pdf/structure/RenderableObject.hpp"

#include <iostream>

namespace vrock::pdf
{
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
            content += content_stream->data->to_string( );

        // uto parser = ContentStreamParser( content, resources, context );
        // text = std::move( parser.text_elements );
        // images = std::move( parser.images );
        // text = std::move( parser.text );
        parsed = true;
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
} // namespace vrock::pdf