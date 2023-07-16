module;

#include "cryptopp/osrng.h"
#include "cryptopp/randpool.h"

import vrock.utils.ByteArray;

module vrock.security.random;

namespace vrock::security
{
    auto generate_random_bytes_non_blocking( size_t n ) -> utils::ByteArray<>
    {
        auto data = utils::ByteArray( n );
        CryptoPP::NonblockingRng rng;
        rng.GenerateBlock( data.data( ), n );
        return data;
    }

    auto generate_random_bytes( size_t n )->utils::ByteArray<>
    {
        auto data = utils::ByteArray( n );
        CryptoPP::RandomPool rng;
        rng.GenerateBlock( data.data( ), n );
        return data;
    }
}