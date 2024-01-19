#pragma once

#include <cstdint>
#include <span>
#include <sstream>
#include <string>

#include <iomanip>

namespace vrock::utils
{
    inline auto to_string( std::span<std::uint8_t> data ) -> std::string
    {
        std::stringstream stream;
        for ( const char c : data )
            stream << c;
        return stream.str( );
    }

    auto to_hex_string( std::span<std::uint8_t> data ) -> std::string
    {
        std::stringstream stream;
        stream << std::hex << std::setfill( '0' );
        for ( const int c : data )
            stream << std::hex << std::setw( 2 ) << c;
        return stream.str( );
    }
} // namespace vrock::utils