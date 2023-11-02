#pragma once

#include "PDFObjects.hpp"

#include <cmath>

namespace vrock::pdf
{
    enum class ColorSpaceType
    {
        RGB,
        Gray,
        CMYK,
        Separation,
        DeviceN,
        Indexed,
        Pattern,
        CalRGB,
        CalGray,
        Lab,
        ICC
    };

    class ColorSpace
    {
    public:
        explicit ColorSpace( ColorSpaceType t ) : type( t )
        {
        }

        virtual auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        virtual auto convert_to_cmyk( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        virtual auto convert_to_gray( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        ColorSpaceType type;

    protected:
        std::shared_ptr<PDFBaseObject> _settings = nullptr;
        std::uint8_t bpp = 8;
        std::int32_t width = 0, height = 0;
    };

    class RGBColorSpace : public ColorSpace
    {
    public:
        RGBColorSpace( ) : ColorSpace( ColorSpaceType::RGB )
        {
        }

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> final
        {
            return data;
        }
    };

    /**
     * @param stride in bit
     * */
    auto get_num( std::uint8_t *data, std::size_t idx, std::uint8_t stride ) -> std::uint8_t;

    class GrayColorSpace : public ColorSpace
    {
    public:
        explicit GrayColorSpace( );

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> params )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    class CMYKColorSpace : public ColorSpace
    {
    public:
        explicit CMYKColorSpace( ) : ColorSpace( ColorSpaceType::CMYK )
        {
        }

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> final
        {
            return data;
        }
    };

    class IndexedColorSpace : public ColorSpace
    {
    public:
        explicit IndexedColorSpace( std::shared_ptr<PDFArray> );

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>>;

    protected:
        std::int32_t highest_value = 0;
        std::shared_ptr<utils::ByteArray<>> map = nullptr;
    };

    auto to_colorspace( std::shared_ptr<PDFBaseObject> ) -> std::shared_ptr<ColorSpace>;
} // namespace vrock::pdf