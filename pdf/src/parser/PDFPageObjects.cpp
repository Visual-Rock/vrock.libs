module;

import vrock.utils.ByteArray;

#include <cstdint>
#include <cstring>
#include <iostream>
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

    PDFImage::PDFImage( std::shared_ptr<PDFStream> stm ) : stream( stm )
    {
        if ( auto w = stream->dict->get<PDFInteger>( "Width" ) )
            width = w->value;
        if ( auto h = stream->dict->get<PDFInteger>( "Height" ) )
            height = h->value;
        if ( auto b = stream->dict->get<PDFInteger>( "BitsPerComponent" ) )
            bpp = b->value;

        // Colorspace
        color_space = to_colorspace( stm->dict->get( "ColorSpace" ) );

        // TODO: implement JPXDecode and DCTDecode correctly
        if ( stream->filters.contains( "JPXDecode" ) || stream->filters.contains( "DCTDecode" ) )
        {
            int w = 0, h = 0, c = 0;
            auto i = stbi_load_from_memory( stream->data->data( ), stream->data->size( ), &w, &h, &c, 3 );
            auto data = std::make_shared<utils::ByteArray<>>( w * c * h );
            std::memcpy( data->data( ), i, data->size( ) );
            stream->data = data;
            stbi_image_free( i );
        }
    }

    auto PDFImage::save( const std::string &path, ImageSaveFormat format ) -> void
    {
        auto rgba = as_rgba( );
        switch ( format )
        {
        case ImageSaveFormat::png:
            stbi_write_png( path.c_str( ), width, height, 4, rgba->data( ), rgba->size( ) / height );
            break;
        }
    }
} // namespace vrock::pdf