module;

import vrock.utils.ByteArray;
import vrock.pdf.PDFBaseObjects;

#include <cmath>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <memory>

module vrock.pdf.PDFBaseObjects;

namespace vrock::pdf
{
    auto GrayColorSpace::convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data,
                                         std::shared_ptr<PDFBaseObject> params ) -> std::shared_ptr<utils::ByteArray<>>
    {
        std::int32_t width = 0, height = 0, bpp = 8;
        if ( auto dict = params->to<PDFDictionary>( ) )
        {
            if ( auto w = dict->get<PDFInteger>( "Width" ) )
                width = w->value;
            if ( auto h = dict->get<PDFInteger>( "Height" ) )
                height = h->value;
            if ( auto b = dict->get<PDFInteger>( "BitsPerComponent" ) )
                bpp = b->value;
        }
        std::shared_ptr<utils::ByteArray<>> converted = std::make_shared<utils::ByteArray<>>( width * height * 3 );
        switch ( bpp )
        {
        case 1:
        case 2:
        case 4: {
            auto max = std::pow( 2, bpp ) - 1;
            auto row_len = ( width + 7 ) >> 3;
            // TODO: std::exec::par
            for ( auto i = 0; i < height; i++ )
            {
                for ( auto j = 0; j < width; j++ )
                {
                    auto curr_idx = i * width + j;
                    // data->data( ) + ( row_len * i ) -> Pointer to the beginning of the row
                    // j -> offset from row start
                    // / max -> map to space between 0.0 and 1.0
                    // * 255 -> convert range between 0.0 - 1.0 to a range between 0 and 255
                    auto num = (uint8_t)( ( get_num( data->data( ) + ( row_len * i ), j, bpp ) / max ) * 255 );
                    converted->data( )[ curr_idx * 3 ] = num;
                    converted->data( )[ curr_idx * 3 + 1 ] = num;
                    converted->data( )[ curr_idx * 3 + 2 ] = num;
                }
            }
            break;
        }
        case 8: {
            for ( auto i = 0; i < height; i++ )
            {
                for ( auto j = 0; j < width; j++ )
                {
                    auto curr_idx = i * width + j;
                    auto num = data->at( curr_idx );
                    converted->data( )[ curr_idx * 3 ] = num;
                    converted->data( )[ curr_idx * 3 + 1 ] = num;
                    converted->data( )[ curr_idx * 3 + 2 ] = num;
                }
            }
            break;
        }
        case 16:
        }
        return converted;
    }

    IndexedColorSpace::IndexedColorSpace( std::shared_ptr<PDFArray> arr ) : ColorSpace( ColorSpaceType::Indexed )
    {
        if ( auto highval = arr->get<PDFInteger>( 2 ) )
            highest_value = highval->value;
        if ( auto str = arr->get<PDFString>( 3 ) )
            map = str->get_byte_array( );
        else if ( auto stream = arr->get<PDFStream>( 3 ) )
            map = stream->data;
    }

    auto IndexedColorSpace::convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data,
                                            std::shared_ptr<PDFBaseObject> params )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        std::int32_t width = 0, height = 0, bpp = 8;
        if ( auto dict = params->to<PDFDictionary>( ) )
        {
            if ( auto w = dict->get<PDFInteger>( "Width" ) )
                width = w->value;
            if ( auto h = dict->get<PDFInteger>( "Height" ) )
                height = h->value;
            if ( auto b = dict->get<PDFInteger>( "BitsPerComponent" ) )
                bpp = b->value;
        }

        auto converted = std::make_shared<utils::ByteArray<>>( width * height * 3 );
        switch ( bpp )
        {
        case 1:
        case 2:
        case 4: {
            auto row_len = ( width + 7 ) >> 3;
            // TODO: std::exec::par
            for ( auto i = 0; i < height; i++ )
            {
                for ( auto j = 0; j < width; j++ )
                {
                    auto curr_idx = i * width + j;
                    auto num = get_num( data->data( ) + ( row_len * i ), j, bpp );
                    if ( num > highest_value ) [[unlikely]]
                        continue;
                    else [[likely]]
                        std::memcpy( converted->data( ) + curr_idx * 3, map->data( ) + num * 3, 3 );
                }
            }
            break;
        }
        case 8: {
            for ( auto i = 0; i < ( width * height ); ++i )
                if ( data->at( i ) > highest_value ) [[unlikely]]
                    continue;
                else [[likely]]
                    std::memcpy( converted->data( ) + i * 3, map->data( ) + data->at( i ) * 3, 3 );
            break;
        }
        case 16:
        }
        return converted;
    }

    auto to_colorspace( std::shared_ptr<PDFBaseObject> obj ) -> std::shared_ptr<ColorSpace>
    {
        if ( obj == nullptr )
            return std::make_shared<RGBColorSpace>( );

        if ( auto color_space = obj->to<PDFArray>( ) )
        {
            if ( auto type = color_space->get<PDFName>( 0 ) )
            {
                if ( type->name == "Indexed" )
                    return std::make_shared<IndexedColorSpace>( color_space );
            }
        }
        else if ( auto color_space = obj->to<PDFName>( ) )
        {
            static std::size_t i;
            std::cout << i++ << " " << color_space->name << std::endl;
            if ( color_space->name == "DeviceRGB" )
                return std::make_shared<RGBColorSpace>( );
            else if ( color_space->name == "DeviceGray" )
                return std::make_shared<GrayColorSpace>( );
            else if ( color_space->name == "DeviceCMYK" )
                return std::make_shared<CMYKColorSpace>( );
        }
        return std::make_shared<RGBColorSpace>( );
    }
} // namespace vrock::pdf