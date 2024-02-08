#pragma once

#include "PDFObjects.hpp"

#include <cmath>

namespace vrock::pdf
{
    // this typedef is used for consistency reasons (change if needed)
    typedef std::uint16_t RGBInt;
    typedef float CMYKFloat;

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

        virtual auto convert_to_rgb( data_t, std::shared_ptr<PDFBaseObject> ) -> data_t = 0;

        virtual auto convert_to_cmyk( data_t, std::shared_ptr<PDFBaseObject> ) -> data_t = 0;

        virtual auto convert_to_gray( data_t, std::shared_ptr<PDFBaseObject> ) -> data_t = 0;

        ColorSpaceType type;

    protected:
        std::shared_ptr<PDFBaseObject> _settings = nullptr;
        std::uint8_t bpp = 8;
        std::int32_t width = 0, height = 0;
    };

    class RGBColorSpace : public ColorSpace
    {
    public:
        RGBColorSpace( );

        auto convert_to_rgb( data_t data, std::shared_ptr<PDFBaseObject> ) -> data_t final
        {
            return data;
        }
        auto convert_to_cmyk( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t final
        {
            return data_t( );
        }
        auto convert_to_gray( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t final
        {
            return data_t( );
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

        auto convert_to_rgb( data_t data, std::shared_ptr<PDFBaseObject> params ) -> data_t final;
        auto convert_to_cmyk( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t override;
        auto convert_to_gray( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t override;
    };

    class CMYKColorSpace : public ColorSpace
    {
    public:
        explicit CMYKColorSpace( ) : ColorSpace( ColorSpaceType::CMYK )
        {
        }

        auto convert_to_rgb( data_t data, std::shared_ptr<PDFBaseObject> ) -> data_t final
        {
            return data;
        }
        auto convert_to_cmyk( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t override
        {
            return data_t( );
        }
        auto convert_to_gray( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t override
        {
            return data_t( );
        }
    };

    class IndexedColorSpace : public ColorSpace
    {
    public:
        explicit IndexedColorSpace( std::shared_ptr<PDFArray> );

        auto convert_to_rgb( data_t, std::shared_ptr<PDFBaseObject> ) -> data_t;
        auto convert_to_cmyk( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t override;
        auto convert_to_gray( data_t ptr, std::shared_ptr<PDFBaseObject> sharedPtr ) -> data_t override;

    protected:
        std::int32_t highest_value = 0;
        data_t map;
    };

    auto to_colorspace( std::shared_ptr<PDFBaseObject> ) -> std::shared_ptr<ColorSpace>;
} // namespace vrock::pdf
