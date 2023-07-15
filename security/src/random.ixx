module;

export import vrock.utils.ByteArray;

import "cryptopp/osrng.h";
import "cryptopp/randpool.h";

export module vrock.security.random;

namespace vrock::security
{
    export auto generate_random_bytes_non_blocking( size_t n ) -> utils::ByteArray<>
     {
         auto data = utils::ByteArray( n );
         CryptoPP::NonblockingRng rng;
         rng.GenerateBlock( data.data( ), n );
         return data;
     }

    export auto generate_random_bytes( size_t n )->utils::ByteArray<>
     {
         auto data = utils::ByteArray( n );
         CryptoPP::RandomPool rng;
         rng.GenerateBlock( data.data( ), n );
         return data;
     }
}