module;

import vrock.utils.ByteArray;

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

    auto PDFFlateFilter::decode( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFDictionary> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        return data;
    }
} // namespace vrock::pdf