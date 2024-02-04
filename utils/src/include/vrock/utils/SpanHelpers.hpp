#pragma once

#include <cstdint>
#include <cstring>
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

    template <typename T>
    auto from_hex_string( std::string_view data ) -> T
    {
        static_assert( false, "from_hex_string not implemented for this type" );
    }

    template <>
    inline auto from_hex_string<std::string>( std::string_view data ) -> std::string
    {
        std::string result;
        result.reserve( data.size( ) / 2 );
        for ( std::size_t i = 0; i < data.size( ); i += 2 )
            result.push_back( static_cast<char>( std::stoul( std::string( data.substr( i, 2 ) ), nullptr, 16 ) ) );
        return result;
    }

    inline auto combine_strings( std::span<std::string> strings, std::size_t size ) -> std::string
    {
        std::string ret( size, '\0' );
        std::size_t offset = 0;
        int i = 0;
        while ( offset < size )
        {
            std::memcpy( ret.data( ) + offset, strings[ i ].data( ), std::min( strings[ i ].size( ), size - offset ) );
            offset += strings[ i ].size( );
            ++i;
        }
        return ret;
    }
} // namespace vrock::utils