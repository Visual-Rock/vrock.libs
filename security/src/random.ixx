module;

#include <cstddef>

export import vrock.utils.ByteArray;

export module vrock.security.random;

namespace vrock::security
{
    export auto generate_random_bytes_non_blocking( std::size_t n ) -> utils::ByteArray<>;

    export auto generate_random_bytes( std::size_t n ) -> utils::ByteArray<>;
} // namespace vrock::security