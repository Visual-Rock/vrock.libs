module;

export import vrock.utils.ByteArray;
#include <cstddef>

export module vrock.security.random;

namespace vrock::security
{
    export auto generate_random_bytes_non_blocking( std::size_t n ) -> utils::ByteArray<>;

    export auto generate_random_bytes( std::size_t n ) -> utils::ByteArray<>;
}