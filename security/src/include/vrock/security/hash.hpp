#pragma once

#include <memory>
#include <vrock/utils/ByteArray.hpp>

#include "typedefs.hpp"

namespace vrock::security
{
    /**
     * @brief hashes the data using the MD5 algorithm
     * @param data data to hash
     * @return hash of the data
     */
    auto md5( byte_span_t data ) -> return_t;

    /// @brief hashes the data using the sha-224 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha224( byte_span_t data ) -> return_t;

    /// @brief hashes the data using the sha-256 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha256( byte_span_t data ) -> return_t;

    /// @brief hashes the data using the sha-384 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha384( byte_span_t data ) -> return_t;

    /// @brief hashes the data using the sha-512 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha512( byte_span_t data ) -> return_t;

    /// @brief hashes the data using the MD5 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto md5( string_view_t data ) -> return_string_t;

    /// @brief hashes the data using the sha-224 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha224( string_view_t data ) -> return_string_t;

    /// @brief hashes the data using the sha-256 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha256( string_view_t data ) -> return_string_t;

    /// @brief hashes the data using the sha-384 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha384( string_view_t data ) -> return_string_t;

    /// @brief hashes the data using the sha-512 algorithm
    /// @param data data to hash
    /// @return hash of the data
    auto sha512( string_view_t data ) -> return_string_t;
} // namespace vrock::security