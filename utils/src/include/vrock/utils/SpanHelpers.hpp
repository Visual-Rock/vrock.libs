#pragma once

#include <cstdint>
#include <span>
#include <sstream>
#include <string>

#include <iomanip>

namespace vrock::utils
{
    inline auto to_string( const std::span<std::uint8_t> data ) -> std::string
    {
        std::stringstream stream;
        for ( const unsigned char c : data )
            stream << c;
        return stream.str( );
    }

    inline auto to_hex_string( const std::span<std::uint8_t> data ) -> std::string
    {
        std::stringstream stream;
        for ( const std::uint8_t c : data )
            stream << std::setfill( '0' ) << std::setw( 2 ) << std::hex << static_cast<unsigned int>( c );
        return stream.str( );
    }

    inline auto to_hex_string( const std::string_view data ) -> std::string
    {
        std::stringstream stream;
        for ( const std::uint8_t c : data )
            stream << std::setfill( '0' ) << std::setw( 2 ) << std::hex << static_cast<unsigned int>( c );
        return stream.str( );
    }
} // namespace vrock::utils