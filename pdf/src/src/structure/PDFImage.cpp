#include "vrock/pdf/structure/PDFImage.hpp"

#include <stb_image_write.h>

#include <utility>

#include <cstring>

namespace vrock::pdf
{
    PDFImage::PDFImage( std::shared_ptr<PDFStream> stm ) : stream( std::move( stm ) )
    {
        if ( auto w = stream->dict->get<PDFInteger>( "Width" ) )
            width = w->value;
        if ( auto h = stream->dict->get<PDFInteger>( "Height" ) )
            height = h->value;
        if ( auto b = stream->dict->get<PDFInteger>( "BitsPerComponent" ) )
            bpp = b->value;

        // Colorspace
        //        color_space = to_colorspace( stm->dict->get( "ColorSpace" ) );
    }

    auto PDFImage::save( const std::string &path, ImageSaveFormat format ) -> void
    {
        auto rgba = as_rgba( );
        switch ( format )
        {
        case ImageSaveFormat::png:
            stbi_write_png( path.c_str( ), width, height, 4, rgba.data( ), rgba.size( ) / height );
            break;
        }
    }

    auto PDFImage::as_rgba( ) -> data_t
    {
        auto rgb = as_rgb( );
        auto converted = data_t( rgb.size( ) / 3 * 4, '\0' );
        for ( auto i = 0; i < rgb.size( ) / 3; i++ )
        {
            std::memcpy( converted.data( ) + 4 * i, rgb.data( ) + 3 * i, 3 );
            converted.at( 4 * i + 3 ) = 0xff;
        }
        // TODO: Image mask Soft Masks...
        return converted;
    }
} // namespace vrock::pdf