#pragma once

#include <cstddef>
#include <memory>
#include <vrock/utils/ByteArray.hpp>

namespace vrock::security
{
    auto generate_random_bytes_non_blocking( std::size_t n ) -> utils::ByteArray<>;

    auto generate_random_bytes( std::size_t n ) -> utils::ByteArray<>;

    auto generate_random_bytes_non_blocking_shared( std::size_t n ) -> std::shared_ptr<utils::ByteArray<>>;

    auto generate_random_bytes_shared( std::size_t n ) -> std::shared_ptr<utils::ByteArray<>>;
} // namespace vrock::security