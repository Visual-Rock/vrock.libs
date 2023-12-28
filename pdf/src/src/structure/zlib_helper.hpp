#pragma once

#include <vrock/utils/ByteArray.hpp>

#include <cstring>
#include <zlib.h>

namespace vrock::pdf
{
    constexpr std::size_t buffer_size = 16384;
    thread_local inline auto buffer = std::make_shared<utils::ByteArray<>>( buffer_size );

    auto inflate( const std::shared_ptr<utils::ByteArray<>> &data, std::size_t new_buffer_size = 16384 )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        std::vector<std::shared_ptr<utils::ByteArray<>>> chunks = { };
        z_stream zs;
        std::memset( &zs, 0, sizeof( zs ) );
        if ( inflateInit( &zs ) != Z_OK )
            throw std::runtime_error( "stream decompression failed" );
        zs.next_in = (Bytef *)data->data( );
        zs.avail_in = data->size( );

        zs.next_out = buffer->data( );
        zs.avail_out = buffer_size;
        int ret = inflate( &zs, 0 );
        if ( ret != Z_OK )
        {
            inflateEnd( &zs );
            if ( ret != Z_STREAM_END )
                throw std::runtime_error( zs.msg );
            return buffer->subarr_shared( 0, zs.total_out );
        }
        chunks.push_back( buffer );
        do
        {
            auto chunk = std::make_shared<utils::ByteArray<>>( new_buffer_size );
            zs.next_out = chunk->data( );
            zs.avail_out = new_buffer_size;
            ret = inflate( &zs, 0 );
            chunks.push_back( chunk );
        } while ( ret == Z_OK );
        inflateEnd( &zs );
        if ( ret != Z_STREAM_END )
            throw std::runtime_error( zs.msg );
        return utils::combine_arrays( chunks, zs.total_out );
    }
} // namespace vrock::pdf