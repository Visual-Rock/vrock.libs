#include "vrock/security.hpp"

#include "cryptopp/osrng.h"
#include "cryptopp/randpool.h"

namespace vrock::security
{
    auto generate_random_bytes_non_blocking( size_t n ) -> utils::ByteArray<>
    {
        auto data = utils::ByteArray( n );
        CryptoPP::NonblockingRng rng;
        rng.GenerateBlock( data.data( ), n );
        return data;
    }

    auto generate_random_bytes( size_t n ) -> utils::ByteArray<>
    {
        auto data = utils::ByteArray( n );
        CryptoPP::RandomPool rng;
        rng.GenerateBlock( data.data( ), n );
        return data;
    }

    auto generate_random_bytes_non_blocking_shared( std::size_t n ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto data = std::make_shared<utils::ByteArray<>>( n );
        CryptoPP::NonblockingRng rng;
        rng.GenerateBlock( data->data( ), n );
        return data;
    }

    auto generate_random_bytes_shared( std::size_t n ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto data = std::make_shared<utils::ByteArray<>>( n );
        CryptoPP::RandomPool rng;
        rng.GenerateBlock( data->data( ), n );
        return data;
    }
} // namespace vrock::security