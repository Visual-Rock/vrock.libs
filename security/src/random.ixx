module;

export import vrock.utils.ByteArray;

#include <cstddef>
#include <memory>

export module vrock.security.random;

namespace vrock::security
{
    export auto generate_random_bytes_non_blocking( std::size_t n ) -> utils::ByteArray<>;

    export auto generate_random_bytes( std::size_t n ) -> utils::ByteArray<>;

    export auto generate_random_bytes_non_blocking_shared( std::size_t n ) -> std::shared_ptr<utils::ByteArray<>>;

    export auto generate_random_bytes_shared( std::size_t n ) -> std::shared_ptr<utils::ByteArray<>>;
} // namespace vrock::security