#include "vrock/pdf/structure/Colorspaces.hpp"
#include <cassert>

namespace vrock::pdf
{
    template <typename T>
    struct RGB
    {
        T r, g, b;
        T max_value;
    };

    template <typename T>
    struct CMYK
    {
        T c, m, y, k;
        T max_value;
    };

    template <typename T>
    struct Gray
    {
        T g;
        T max_value;
    };

    typedef RGB<float> RGBf;
    typedef RGB<std::uint16_t> RGBi;

    typedef CMYK<float> CMYKf;
    typedef CMYK<std::uint16_t> CMYKi;

    typedef Gray<float> Grayf;
    typedef Gray<std::uint16_t> Grayi;

    template <typename T>
    auto to_rgb( T t ) -> RGBf
    {
        static_assert( true, "this should not happen!" );
    }

    template <>
    auto to_rgb<RGBi>( RGBi rgbi ) -> RGBf
    {
        RGBf rgb{ };
        rgb.r = (float)rgbi.r / (float)rgbi.max_value;
        rgb.g = (float)rgbi.g / (float)rgbi.max_value;
        rgb.b = (float)rgbi.b / (float)rgbi.max_value;
        rgb.max_value = 1.0f;
        return rgb;
    }

    template <>
    auto to_rgb<CMYKf>( CMYKf cmyk ) -> RGBf
    {
        RGBf rgb{ };
        rgb.r = 1.0f - std::min( 1.0f, cmyk.c + cmyk.k );
        rgb.g = 1.0f - std::min( 1.0f, cmyk.m + cmyk.k );
        rgb.b = 1.0f - std::min( 1.0f, cmyk.y + cmyk.k );
        return rgb;
    }

    template <>
    auto to_rgb<Grayf>( Grayf gray ) -> RGBf
    {
        return { gray.g, gray.g, gray.g };
    }

    template <typename T>
    auto to_cmyk( T t ) -> CMYKf
    {
        static_assert( true, "this should not happen!" );
    }

    template <>
    auto to_cmyk<RGBf>( RGBf rgb ) -> CMYKf
    {
        float c, m, y, k;
        if ( rgb.max_value == 1.0f )
        {
            c = 1.0f - rgb.r;
            m = 1.0f - rgb.g;
            y = 1.0f - rgb.b;
            k = std::min( { c, m, y } );
        }
        else
        {
        }
        return CMYKf{ };
    }

    RGBColorSpace::RGBColorSpace( ) : ColorSpace( ColorSpaceType::RGB )
    {
    }
    //
    //    auto RGBColorSpace::convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> )
    //        -> std::shared_ptr<utils::ByteArray<>>
    //    {
    //        return data;
    //    }
    //
    //    auto RGBColorSpace::convert_to_cmyk( std::shared_ptr<utils::ByteArray<>> data,
    //                                         std::shared_ptr<PDFBaseObject> sharedPtr )
    //        -> std::shared_ptr<utils::ByteArray<>>
    //    {
    //        auto processed = std::make_shared<utils::ByteArray<>>( ( data->size( ) / 3 ) * 4 * sizeof( CMYKFloat ) );
    //
    //        CMYKFloat r, g, b, k;
    //        std::size_t rgb = 0, cmyk = 0;
    //        for ( int i = 0; i < data->size( ) / 3; ++i )
    //        {
    //            rgb = i * 3;
    //            cmyk = i * 4 * sizeof( CMYKFloat );
    //
    //            r = (CMYKFloat)data->at( rgb ) / 255;
    //            g = (CMYKFloat)data->at( rgb + 1 ) / 255;
    //            b = (CMYKFloat)data->at( rgb + 2 ) / 255;
    //
    //            k = std::max( { r, g, b } );
    //
    //            processed->at( cmyk ) = ( 1.0f - r - k ) / ( 1 - k );
    //        }
    //
    //        return std::shared_ptr<utils::ByteArray<>>( );
    //    }

    //    auto RGBColorSpace::convert_to_gray( std::shared_ptr<utils::ByteArray<>> ptr,
    //                                         std::shared_ptr<PDFBaseObject> sharedPtr )
    //        -> std::shared_ptr<utils::ByteArray<>>
    //    {
    //        return std::shared_ptr<utils::ByteArray<>>( );
    //    }

    GrayColorSpace::GrayColorSpace( ) : ColorSpace( ColorSpaceType::Gray )
    {
    }

    auto GrayColorSpace::convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data,
                                         std::shared_ptr<PDFBaseObject> params ) -> std::shared_ptr<utils::ByteArray<>>
    {
        return std::shared_ptr<utils::ByteArray<>>( );
    }

    std::shared_ptr<utils::ByteArray<>> GrayColorSpace::convert_to_cmyk( std::shared_ptr<utils::ByteArray<>> ptr,
                                                                         std::shared_ptr<PDFBaseObject> sharedPtr )
    {
        return std::shared_ptr<utils::ByteArray<>>( );
    }
    std::shared_ptr<utils::ByteArray<>> GrayColorSpace::convert_to_gray( std::shared_ptr<utils::ByteArray<>> ptr,
                                                                         std::shared_ptr<PDFBaseObject> sharedPtr )
    {
        return std::shared_ptr<utils::ByteArray<>>( );
    }

    auto get_num( std::uint8_t *data, std::size_t idx, std::uint8_t stride ) -> std::uint8_t
    {
        auto bit_offset = idx * stride;
        auto byte_offset = bit_offset / 8;
        auto offset = 7 - ( bit_offset % 8 );
        auto mask = ( ( (uint8_t)std::pow( 2, stride ) ) - 1 ) << offset;
        return ( data[ byte_offset ] & mask ) >> offset;
    }
    std::shared_ptr<utils::ByteArray<>> IndexedColorSpace::convert_to_cmyk( std::shared_ptr<utils::ByteArray<>> ptr,
                                                                            std::shared_ptr<PDFBaseObject> sharedPtr )
    {
        return std::shared_ptr<utils::ByteArray<>>( );
    }
    std::shared_ptr<utils::ByteArray<>> IndexedColorSpace::convert_to_gray( std::shared_ptr<utils::ByteArray<>> ptr,
                                                                            std::shared_ptr<PDFBaseObject> sharedPtr )
    {
        return std::shared_ptr<utils::ByteArray<>>( );
    }
} // namespace vrock::pdf