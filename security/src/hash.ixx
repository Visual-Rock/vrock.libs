module;

import vrock.utils.ByteArray;

#include <memory>

export module vrock.security.hash;

namespace vrock::security
{
    /// @brief hashes the data using the MD5 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto md5( const utils::ByteArray<> &data ) -> utils::ByteArray<>;

    /// @brief hashes the data using the sha-224 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha224( const utils::ByteArray<> &data ) -> utils::ByteArray<>;

    /// @brief hashes the data using the sha-256 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha256( const utils::ByteArray<> &data ) -> utils::ByteArray<>;

    /// @brief hashes the data using the sha-384 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha384( const utils::ByteArray<> &data ) -> utils::ByteArray<>;

    /// @brief hashes the data using the sha-512 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha512( const utils::ByteArray<> &data ) -> utils::ByteArray<>;

    /// @brief hashes the data using the MD5 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto md5( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>;

    /// @brief hashes the data using the sha-224 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha224( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>;

    /// @brief hashes the data using the sha-256 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha256( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>;

    /// @brief hashes the data using the sha-384 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha384( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>;

    /// @brief hashes the data using the sha-512 algorithm
    /// @param data data to hash
    /// @return hash of the data
    export auto sha512( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>;
} // namespace vrock::security