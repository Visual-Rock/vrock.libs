#include "vrock/security.hpp"

#include "cryptopp/osrng.h"
#include "cryptopp/randpool.h"

namespace vrock::security
{
    auto generate_random_bytes_non_blocking( const size_t n ) -> return_t
    {
        return_t data;
        data.resize( n );
        CryptoPP::NonblockingRng rng;
        rng.GenerateBlock( data.data( ), n );
        return data;
    }

    auto generate_random_bytes( const size_t n ) -> return_t
    {
        return_t data;
        data.resize( n );
        CryptoPP::RandomPool rng;
        rng.GenerateBlock( data.data( ), n );
        return data;
    }
} // namespace vrock::security