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
        explicit PDFImage( std::shared_ptr<PDFStream> stm );

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

        inline auto as_rgb( ) -> data_t
        {
            return color_space->convert_to_rgb( data_t( stream->data ), stream->dict );
        }

        auto as_rgba( ) -> data_t;

    private:
        std::int32_t width = 0, height = 0, channel = 3;
        std::uint8_t bpp = 8;
        std::shared_ptr<PDFStream> stream;
        std::shared_ptr<ColorSpace> color_space;
    };
} // namespace vrock::pdf