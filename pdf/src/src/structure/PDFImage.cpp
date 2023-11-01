#include "vrock/pdf/structure/PDFImage.hpp"

#include <stb_image_write.h>

namespace vrock::pdf
{
    PDFImage::PDFImage( std::shared_ptr<PDFStream> stm ) : stream( stm )
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
            stbi_write_png( path.c_str( ), width, height, 4, rgba->data( ), rgba->size( ) / height );
            break;
        }
    }
} // namespace vrock::pdf