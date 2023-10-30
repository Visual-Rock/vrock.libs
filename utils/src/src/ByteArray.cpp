#include "vrock/utils/ByteArray.hpp"

namespace vrock::utils
{
    auto combine_arrays( std::vector<ByteArray<>> &arrs, std::size_t size ) -> ByteArray<>
    {
        ByteArray<> ret( size );
        std::size_t offset = 0;
        int i = 0;
        while ( offset < size )
        {
            std::memcpy( ret.data( ) + offset, arrs[ i ].data( ), std::min( arrs[ i ].size( ), size - offset ) );
            offset += arrs[ i ].size( );
            ++i;
        }
        return ret;
    }

    auto combine_arrays( std::vector<std::shared_ptr<ByteArray<>>> &arrs, std::size_t size )
        -> std::shared_ptr<ByteArray<>>
    {
        auto ret = std::make_shared<ByteArray<>>( size );
        std::size_t offset = 0;
        int i = 0;
        while ( offset < size )
        {
            std::memcpy( ret->data( ) + offset, arrs[ i ]->data( ), std::min( arrs[ i ]->size( ), size - offset ) );
            offset += arrs[ i ]->size( );
            ++i;
        }
        return ret;
    }

    auto combine_arrays( std::vector<ByteArray<>> &arrays ) -> ByteArray<>
    {
        std::size_t size = 0;
        std::for_each( arrays.begin( ), arrays.end( ), [ & ]( const ByteArray<> &ba ) { size += ba.size( ); } );
        return combine_arrays( arrays, size );
    }

    auto combine_arrays( std::vector<std::shared_ptr<ByteArray<>>> &arrays ) -> std::shared_ptr<ByteArray<>>
    {
        std::size_t size = 0;
        std::for_each( arrays.begin( ), arrays.end( ),
                       [ & ]( const std::shared_ptr<ByteArray<>> &ba ) { size += ba->size( ); } );
        return combine_arrays( arrays, size );
    }

    auto from_hex_string( const std::string &str ) -> ByteArray<>
    {
        std::string s = str + ( ( str.length( ) % 2 == 1 ) ? "0" : "" ); // Append zero if needed
        auto data = ByteArray<>( s.length( ) / 2 );

        for ( size_t i = 0; i < data.size( ); ++i )
            data[ i ] = std::stoul( s.substr( i * 2, 2 ), nullptr, 16 );

        return data;
    }

    auto from_hex_string_shared( const std::string &str ) -> std::shared_ptr<ByteArray<>>
    {
        std::string s = str + ( ( str.length( ) % 2 == 1 ) ? "0" : "" ); // Append zero if needed
        auto data = std::make_shared<ByteArray<>>( s.length( ) / 2 );
        for ( size_t i = 0; i < data->size( ); ++i )
            data->at( i ) = std::stoul( s.substr( i * 2, 2 ), nullptr, 16 );
        return data;
    }

} // namespace vrock::utils