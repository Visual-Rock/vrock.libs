#pragma once

#include "Colorspaces.hpp"
#include "PDFObjects.hpp"
#include "PDFStreams.hpp"

namespace vrock::pdf
{
    enum class ImageSaveFormat
    {
        png
    };

    class PDFImage
    {
    public:
        PDFImage( std::shared_ptr<PDFStream> stm );

        auto save( const std::string &path, ImageSaveFormat format = ImageSaveFormat::png ) -> void;

        inline auto get_width( ) const -> std::int32_t
        {
            return width;
        }

        inline auto get_height( ) const -> std::int32_t
        {
            return height;
        }

        inline auto get_bits_per_pixel( ) const -> std::int32_t
        {
            return bpp;
        }

        inline auto as_rgb( ) -> std::shared_ptr<utils::ByteArray<>>
        {
            return color_space->convert_to_rgb( stream->data, stream->dict );
        }

        auto as_rgba( ) -> std::shared_ptr<utils::ByteArray<>>
        {
            auto rgb = as_rgb( );
            auto converted = std::make_shared<utils::ByteArray<>>( ( rgb->size( ) / 3 ) * 4 );
            for ( auto i = 0; i < rgb->size( ) / 3; i++ )
            {
                std::memcpy( converted->data( ) + ( 4 * i ), rgb->data( ) + ( 3 * i ), 3 );
                converted->at( ( 4 * i ) + 3 ) = 0xff;
            }
            // TODO: Image mask Soft Masks...
            return converted;
        }

    private:
        std::int32_t width = 0, height = 0, channel = 3;
        std::uint8_t bpp = 8;
        std::shared_ptr<PDFStream> stream;
        std::shared_ptr<ColorSpace> color_space;
    };
} // namespace vrock::pdf