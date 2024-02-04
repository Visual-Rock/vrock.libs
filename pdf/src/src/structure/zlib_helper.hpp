#pragma once

#include <vrock/utils/ByteArray.hpp>

#include <cstring>
#include <zlib.h>

#include <vrock/utils/SpanHelpers.hpp>

namespace vrock::pdf
{
    constexpr std::size_t buffer_size = 16384;
    thread_local inline auto buffer = data_t( buffer_size, '\0' );

    auto inflate( in_data_t data, std::size_t new_buffer_size = 16384 ) -> data_t
    {
        std::vector<data_t> chunks = { };
        z_stream zs;
        std::memset( &zs, 0, sizeof( zs ) );
        if ( inflateInit( &zs ) != Z_OK )
            throw std::runtime_error( "stream decompression failed" );
        zs.next_in = (Bytef *)data.data( );
        zs.avail_in = data.size( );

        zs.next_out = (Bytef *)buffer.data( );
        zs.avail_out = buffer_size;
        int ret = inflate( &zs, 0 );
        if ( ret != Z_OK )
        {
            inflateEnd( &zs );
            if ( ret != Z_STREAM_END )
                throw std::runtime_error( zs.msg );
            return buffer.substr( 0, zs.total_out );
        }
        chunks.push_back( buffer );
        do
        {
            auto chunk = data_t( new_buffer_size, '\0' );
            zs.next_out = (Bytef *)chunk.data( );
            zs.avail_out = new_buffer_size;
            ret = inflate( &zs, 0 );
            chunks.push_back( chunk );
        } while ( ret == Z_OK );
        inflateEnd( &zs );
        if ( ret != Z_STREAM_END )
            throw std::runtime_error( zs.msg );
        return utils::combine_strings( chunks, zs.total_out );
    }
} // namespace vrock::pdf