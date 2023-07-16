module;

export module vrock.security.hash;

export import vrock.utils.ByteArray;

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
} // namespace vrock::security