#pragma once

#include "typedefs.hpp"

namespace vrock::security
{
    /**
     * @brief Generates a specified number of random bytes in a non-blocking manner.
     *
     * This function uses a non-blocking method to generate a sequence of random bytes
     * with the specified size. The generated bytes are stored in a std::vector<std::uint8_t>
     * and returned to the caller.
     *
     * @param n The number of random bytes to generate.
     * @return A std::vector<std::uint8_t> containing the generated random bytes.
     */
    auto generate_random_bytes_non_blocking( std::size_t n ) -> return_t;

    /**
     * @brief Generates a specified number of random bytes.
     *
     * This function generates a sequence of random bytes with the specified size.
     * The generated bytes are stored in a std::vector<std::uint8_t> and returned to the caller.
     * Note that this function may block if the underlying random number generation process
     * involves blocking operations.
     *
     * @param n The number of random bytes to generate.
     * @return A std::vector<std::uint8_t> containing the generated random bytes.
     */
    auto generate_random_bytes( std::size_t n ) -> return_t;
} // namespace vrock::security