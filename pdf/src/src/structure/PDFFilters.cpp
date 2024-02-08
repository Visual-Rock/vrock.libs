#include "vrock/pdf/structure/PDFFilters.hpp"

#include "vrock/pdf/structure/PDFObjects.hpp"

#include "stb_image.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "zlib_helper.hpp"

#include <vrock/utils/SpanHelpers.hpp>

namespace vrock::pdf
{
    static std::vector<char> _ignore = std::vector<char>( { '\0', '\t', '\n', '\f', '\r', ' ', '>' } );

    auto PDFASCIIFilter::encode( in_data_t data, std::shared_ptr<PDFDictionary> ) -> data_t
    {
        auto encoded = utils::to_hex_string( data );
        return encoded + '>';
    }

    auto PDFASCIIFilter::decode( in_data_t data, std::shared_ptr<PDFDictionary> ) -> data_t
    {
        // Filter all characters that can appear but are not hex
        std::stringstream ss;
        for ( size_t i = 0; i < data.size( ); ++i )
            if ( std::find( _ignore.begin( ), _ignore.end( ), data[ i ] ) == _ignore.end( ) )
                ss << data[ i ];
        // convert to data
        return utils::from_hex_string<std::string>( ss.str( ) );
    }

    auto PDFFlateFilter::encode( in_data_t data, std::shared_ptr<PDFDictionary> ) -> data_t
    {
        return data_t( data );
    }

    auto predict_png( const in_data_t &data, int col, int bpc, int bpp ) -> data_t;

    auto PDFFlateFilter::decode( in_data_t data, std::shared_ptr<PDFDictionary> params ) -> data_t
    {
        int predictor = 1;
        int colors = 1;
        int bit_per_component = 8;
        int columns = 1;
        if ( auto pred = params->get<PDFNumber>( "Predictor" ) )
            predictor = pred->as_int( );
        if ( auto color = params->get<PDFNumber>( "Colors" ) )
            colors = color->as_int( );
        if ( auto bpc = params->get<PDFNumber>( "BitsPerComponent" ) )
            bit_per_component = bpc->as_int( );
        if ( auto col = params->get<PDFNumber>( "Columns" ) )
            columns = col->as_int( );
        auto pixel_length = ( colors * bit_per_component + 7 ) / 8;
        auto inflated = inflate( data );
        // apply predictor
        switch ( predictor )
        {
        case 1:
            return inflated;
        case 10:
        case 11:
        case 12:
        case 13:
            return predict_png( inflated, columns, bit_per_component, pixel_length );
        default:
            // log::get_logger( "pdf" )->log->info( "unknown predictor {}", predictor );
            return inflated;
        }
    }

    auto PDFDCTFilter::encode( in_data_t data, std::shared_ptr<PDFDictionary> ) -> data_t
    {
        return data_t( data );
    }

    auto PDFDCTFilter::decode( in_data_t data, std::shared_ptr<PDFDictionary> dict ) -> data_t
    {
        if ( auto color_transform = dict->get<PDFInteger>( "ColorTransform" ) )
            std::cout << color_transform->value << std::endl;
        int w = 0, h = 0, c = 0;
        auto i = stbi_load_from_memory( reinterpret_cast<unsigned char const *>( data.data( ) ), data.size( ), &w, &h,
                                        &c, 3 );
        auto d = data_t( w * c * h, '\0' );
        std::memcpy( (void *)data.data( ), i, data.size( ) );
        stbi_image_free( i );
        return d;
    }

    auto predict_png( const in_data_t &data, int col, int bpc, int bpp ) -> data_t
    {
        auto row_len = ( col * bpp ) + 1;
        size_t len = col * bpp;
        auto rows = ( data.size( ) / row_len );
        auto decoded = data_t( rows * col, '\0' );

        for ( size_t i = 0; i < rows; i++ )
        {
            auto pred = data.data( )[ i * row_len ];
            auto row_start_o = row_len * i + 1;
            auto row_start_d = len * i;

            switch ( pred )
            {
            case 0:
                std::memcpy( decoded.data( ) + row_start_d, data.data( ) + row_start_o, len );
            case 1:
                std::memcpy( decoded.data( ) + row_start_d, data.data( ) + row_start_o, bpp );
                for ( size_t j = bpp; j < len; j++ )
                    decoded.at( row_start_d + j ) = data.at( row_start_o + j ) + decoded.at( row_start_d + j - bpp );
                break;
            case 2:
                if ( i != 0 )
                    for ( int j = 0; j < col; j++ )
                        decoded.at( row_start_d + j ) =
                            data.at( row_start_o + j ) + decoded.at( row_start_d - len + j );
                else
                    std::memcpy( decoded.data( ), data.data( ) + 1, col );
                break;
            default:
                vrock::log::get_logger( "pdf" )->error( "PNG predictor {} not supported!", pred );
                break;
            }
        }
        return decoded;
    }
} // namespace vrock::pdf