module;

import vrock.utils.ByteArray;

#include "stb_image.h"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "ZLibHelper.hpp"

module vrock.pdf.PDFBaseObjects;

import vrock.pdf.PDFObjectParser;

namespace vrock::pdf
{
    static std::vector<char> _ignore = std::vector<char>( { '\0', '\t', '\n', '\f', '\r', ' ', '>' } );

    auto PDFASCIIFilter::encode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto encoded = data->to_hex_string( );
        return std::make_shared<utils::ByteArray<>>( encoded + ">" );
    }

    auto PDFASCIIFilter::decode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        // Filter all characters that can appear but are not hex
        std::stringstream ss;
        for ( size_t i = 0; i < data->size( ); ++i )
            if ( std::find( _ignore.begin( ), _ignore.end( ), (char)data->data( )[ i ] ) == _ignore.end( ) )
                ss << data->data( )[ i ];
        // convert to data
        return utils::from_hex_string_shared( ss.str( ) );
    }

    auto PDFFlateFilter::encode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        return data;
    }

    auto predict_png( const std::shared_ptr<utils::ByteArray<>> &data, int col, int bpc, int bpp )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto PDFFlateFilter::decode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> params )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        int predictor = 1;
        int colors = 1;
        int bit_per_component = 8;
        int columns = 1;
        if ( auto pred = params->get<PDFInteger>( "Predictor" ) )
            predictor = pred->value;
        if ( auto color = params->get<PDFInteger>( "Colors" ) )
            colors = color->value;
        if ( auto bpc = params->get<PDFInteger>( "BitsPerComponent" ) )
            bit_per_component = bpc->value;
        if ( auto col = params->get<PDFInteger>( "Columns" ) )
            columns = col->value;
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

    auto PDFDCTFilter::encode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        return data;
    }

    auto PDFDCTFilter::decode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> dict )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        if ( auto color_transform = dict->get<PDFInteger>( "ColorTransform" ) )
            std::cout << color_transform->value << std::endl;
        int w = 0, h = 0, c = 0;
        auto i = stbi_load_from_memory( data->data( ), data->size( ), &w, &h, &c, 3 );
        auto d = std::make_shared<utils::ByteArray<>>( w * c * h );
        std::memcpy( data->data( ), i, data->size( ) );
        stbi_image_free( i );
        return d;
    }

    auto predict_png( const std::shared_ptr<utils::ByteArray<>> &data, int col, int bpc, int bpp )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto row_len = ( col * bpp ) + 1;
        size_t len = col * bpp;
        auto rows = ( data->size( ) / row_len );
        auto decoded = std::make_shared<utils::ByteArray<>>( rows * col );

        for ( size_t i = 0; i < rows; i++ )
        {
            auto pred = data->data( )[ i * row_len ];
            auto row_start_o = row_len * i + 1;
            auto row_start_d = len * i;

            switch ( pred )
            {
            case 0:
                memcpy( decoded->data( ) + row_start_d, data->data( ) + row_start_o, len );
            case 1:
                memcpy( decoded->data( ) + row_start_d, data->data( ) + row_start_o, bpp );
                for ( size_t j = bpp; j < len; j++ )
                    decoded->at( row_start_d + j ) = data->at( row_start_o + j ) + decoded->at( row_start_d + j - bpp );
                break;
            case 2:
                if ( i != 0 )
                    for ( int j = 0; j < col; j++ )
                        decoded->at( row_start_d + j ) =
                            data->at( row_start_o + j ) + decoded->at( row_start_d - len + j );
                else
                    memcpy( decoded->data( ), data->data( ) + 1, col );
                break;
            default:
                // vrock::log::get_logger( "pdf" )->log->error( "PNG predictor {} not supported!", pred );
                break;
            }
        }
        return decoded;
    }

} // namespace vrock::pdf