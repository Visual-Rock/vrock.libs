module;

import vrock.utils.ByteArray;

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "stb_image.h"
#include "stb_image_write.h"

module vrock.pdf.PDFBaseObjects;

namespace vrock::pdf
{
    static std::unordered_map<std::string, FontType> font_types = { { "Type1", FontType::Type1 },
                                                                    { "TrueType", FontType::TrueType } };

    Font::Font( std::shared_ptr<PDFDictionary> dict )
    {
        if ( auto type = dict->get<PDFName>( "Subtype" ) )
            if ( font_types.contains( type->name ) )
                font_type = font_types[ type->name ];

        if ( auto first = dict->get<PDFInteger>( "FirstChar" ) )
            first_char = first->value;
        if ( auto last = dict->get<PDFInteger>( "FirstChar" ) )
            last_char = last->value;
        if ( auto base = dict->get<PDFName>( "BaseFont" ) )
            base_font = base->name;
        if ( auto name = dict->get<PDFName>( "Name" ) )
            font_name = name->name;
        if ( auto w = dict->get<PDFArray>( "Widths" ) )
        {
            widths.reserve( w->value.size( ) );
            for ( const auto &v : w->value )
                if ( auto val = v->to<PDFInteger>( ) )
                    widths.emplace_back( val->value );
        }
    }

    Image::Image( std::shared_ptr<PDFStream> stm ) : stream( stm )
    {
        if ( auto w = stream->dict->get<PDFInteger>( "Width" ) )
            width = w->value;
        if ( auto h = stream->dict->get<PDFInteger>( "Height" ) )
            height = h->value;
        if ( auto b = stream->dict->get<PDFInteger>( "BitsPerComponent" ) )
            bpp = b->value;

        // Colorspace
        color_space = to_colorspace( stm->dict->get( "ColorSpace" ) );
    }

    auto Image::save( const std::string &path, ImageSaveFormat format ) -> void
    {
        auto rgba = as_rgba( );
        switch ( format )
        {
        case ImageSaveFormat::png:
            stbi_write_png( path.c_str( ), width, height, 4, rgba->data( ), rgba->size( ) / height );
            break;
        }
    }

    ResourceDictionary::ResourceDictionary( std::shared_ptr<PDFDictionary> dict_, std::shared_ptr<PDFContext> ctx )
        : dict( std::move( dict_ ) ), context( std::move( ctx ) )
    {
        if ( auto ext_g_states = dict->get<PDFDictionary>( "ExtGState" ) )
            for ( auto [ k, v ] : ext_g_states->dict )
                if ( auto val = ext_g_states->get<PDFDictionary>( k->name ) ) // to avoid getting PDFRef
                    ext_g_state[ k->name ] = val;

        if ( auto xobjects = dict->get<PDFDictionary>( "XObject" ) )
        {
            for ( auto [ k, v ] : xobjects->dict )
            {
                if ( auto val = xobjects->get( k->name ) )
                {
                    std::string type;
                    if ( auto stm = val->to<PDFStream>( ) )
                        if ( auto t = stm->dict->get<PDFName>( "Subtype" ) )
                            type = t->name;
                    switch ( type.size( ) )
                    {
                    case 4: {
                        // TODO: Form
                        break;
                    }
                    case 0:
                    case 5: {
                        images[ k->name ] = std::make_shared<Image>( val->to<PDFStream>( ) );
                    }
                    }
                }
            }
        }

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

    ResourceDictionary::~ResourceDictionary( )
    {
    }
} // namespace vrock::pdf