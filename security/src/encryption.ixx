module;

import vrock.utils.ByteArray;

export module vrock.security.encryption;

namespace vrock::security
{
    export enum class Padding
    {
        NO_PADDING,
        ZEROS_PADDING,
        PKCS_PADDING,
        W3C_PADDING,
        ONE_AND_ZEROS_PADDING
    };

    /// @brief Encrypts the data with AES in GCM mode
    /// @param data data to encrypt
    /// @param key key for the encryption
    /// @param iv initialization vector
    /// @param authentication_data additional authentication data
    /// @return the encrypted result
    export auto decrypt_aes_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, const utils::ByteArray<> &authentication_data )
        -> utils::ByteArray<>;

    /// @brief Decrypts the data with AES in GCM mode
    /// @param data data to decrypt
    /// @param key key for the decryption
    /// @param iv initialization vector
    /// @param authentication_data additional authentication data
    /// @return decrypted result
    export auto encrypt_aes_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, const utils::ByteArray<> &authentication_data )
        -> utils::ByteArray<>;

    /// @brief encrypts data in AES ECB mode
    /// @param data data to encrypt
    /// @param key key for the encryption
    /// @param padding padding option for encryption
    /// @return the encrypted data
    export auto encrypt_aes_ecb( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 Padding padding = Padding::NO_PADDING ) -> utils::ByteArray<>;

    /// @brief decrypts data in AES ECB mode
    /// @param data data to decrypt
    /// @param key key for the decryption
    /// @param padding padding option for decryption
    /// @return the decrypted data
    export auto decrypt_aes_ecb( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 Padding padding = Padding::NO_PADDING ) -> utils::ByteArray<>;

    /// @brief Encrypts the data with AES in CBC mode
    /// @param data data to encrypt
    /// @param key key for the encryption
    /// @param iv initialization vector
    /// @param padding padding option for encryption
    /// @return the encrypted result
    export auto encrypt_aes_cbc( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, Padding padding = Padding::NO_PADDING )
        -> utils::ByteArray<>;

    /// @brief Decrypts the data with AES in CBC mode
    /// @param data data to decrypt
    /// @param key key for the decryption
    /// @param iv initialization vector
    /// @param padding padding option for decryption
    /// @return the decrypted result
    export auto decrypt_aes_cbc( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, Padding padding = Padding::NO_PADDING )
        -> utils::ByteArray<>;

    /// @brief encrypts data using the RC4 algorithm
    /// @param data data to encrypt
    /// @param key key for encryption
    /// @return encrypted data
    export auto encrypt_rc4( const utils::ByteArray<> &data, const utils::ByteArray<> &key ) -> utils::ByteArray<>;

    /// @brief decrypts data using the RC4 algorithm
    /// @param data data to decrypt
    /// @param key key for decryption
    /// @return decrypted data
    export auto decrypt_rc4( const utils::ByteArray<> &data, const utils::ByteArray<> &key ) -> utils::ByteArray<>;
} // namespace vrock::security