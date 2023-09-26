module;

import vrock.utils.ByteArray;

#include <memory>

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

    /**
     * Encrypt the data with AES in GCM mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param iv initialization vector
     * @param authentication_data additional authentication data
     *
     * @return Encrypted result
     */
    export auto encrypt_aes_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, const utils::ByteArray<> &authentication_data )
        -> utils::ByteArray<>;

    /**
     * Decrypts the data with AES in GCM mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param iv initialization vector
     * @param authentication_data additional authentication data
     *
     * @return Decrypted result
     */
    export auto decrypt_aes_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, const utils::ByteArray<> &authentication_data )
        -> utils::ByteArray<>;

    /**
     * Encrypt the data with AES in ECB mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param padding Padding schema
     *
     * @return Encrypted result
     */
    export auto encrypt_aes_ecb( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 Padding padding = Padding::NO_PADDING ) -> utils::ByteArray<>;

    /**
     * Decrypts the data with AES in ECB mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param padding Padding schema
     *
     * @return Decrypted result
     */
    export auto decrypt_aes_ecb( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 Padding padding = Padding::NO_PADDING ) -> utils::ByteArray<>;

    /**
     * Encrypt the data with AES in GCM mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param iv initialization vector
     * @param padding Padding schema
     *
     * @return Encrypted result
     */
    export auto encrypt_aes_cbc( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, Padding padding = Padding::NO_PADDING )
        -> utils::ByteArray<>;

    /**
     * Decrypts the data with AES in CBC mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param iv initialization vector
     * @param padding Padding schema
     *
     * @return Decrypted result
     */
    export auto decrypt_aes_cbc( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                                 const utils::ByteArray<> &iv, Padding padding = Padding::NO_PADDING )
        -> utils::ByteArray<>;

    /**
     * Encrypt the data with RC4 (unsafe)
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     *
     * @return Encrypted result
     */
    export auto encrypt_rc4( const utils::ByteArray<> &data, const utils::ByteArray<> &key ) -> utils::ByteArray<>;

    /**
     * Decrypts the data with RC4 (unsafe)
     *
     * @param data data to decrypt
     * @param key key for the decryption
     *
     * @return Decrypted result
     */
    export auto decrypt_rc4( const utils::ByteArray<> &data, const utils::ByteArray<> &key ) -> utils::ByteArray<>;

    /**
     * Encrypt the data with AES in GCM mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param iv initialization vector
     * @param authentication_data additional authentication data
     *
     * @return Encrypted result
     */
    export auto encrypt_aes_gcm( const std::shared_ptr<utils::ByteArray<>> &data,
                                 const std::shared_ptr<utils::ByteArray<>> &key,
                                 const std::shared_ptr<utils::ByteArray<>> &iv,
                                 const std::shared_ptr<utils::ByteArray<>> &authentication_data = nullptr )
        -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Decrypts the data with AES in GCM mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param iv initialization vector
     * @param authentication_data additional authentication data
     *
     * @return Decrypted result
     */
    export auto decrypt_aes_gcm( const std::shared_ptr<utils::ByteArray<>> &data,
                                 const std::shared_ptr<utils::ByteArray<>> &key,
                                 const std::shared_ptr<utils::ByteArray<>> &iv,
                                 const std::shared_ptr<utils::ByteArray<>> &authentication_data = nullptr )
        -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Encrypt the data with AES in ECB mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param padding Padding schema
     *
     * @return Encrypted result
     */
    export auto encrypt_aes_ecb( const std::shared_ptr<utils::ByteArray<>> &data,
                                 const std::shared_ptr<utils::ByteArray<>> &key, Padding padding = Padding::NO_PADDING )
        -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Decrypts the data with AES in ECB mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param padding Padding schema
     *
     * @return Decrypted result
     */
    export auto decrypt_aes_ecb( const std::shared_ptr<utils::ByteArray<>> &data,
                                 const std::shared_ptr<utils::ByteArray<>> &key, Padding padding = Padding::NO_PADDING )
        -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Encrypt the data with AES in GCM mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param iv initialization vector
     * @param padding Padding schema
     *
     * @return Encrypted result
     */
    export auto encrypt_aes_cbc( const std::shared_ptr<utils::ByteArray<>> &data,
                                 const std::shared_ptr<utils::ByteArray<>> &key,
                                 const std::shared_ptr<utils::ByteArray<>> &iv, Padding padding = Padding::NO_PADDING )
        -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Decrypts the data with AES in CBC mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param iv initialization vector
     * @param padding Padding schema
     *
     * @return Decrypted result
     */
    export auto decrypt_aes_cbc( const std::shared_ptr<utils::ByteArray<>> &data,
                                 const std::shared_ptr<utils::ByteArray<>> &key,
                                 const std::shared_ptr<utils::ByteArray<>> &iv, Padding padding = Padding::NO_PADDING )
        -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Encrypt the data with RC4 (unsafe)
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     *
     * @return Encrypted result
     */
    export auto encrypt_rc4( const std::shared_ptr<utils::ByteArray<>> &data,
                             const std::shared_ptr<utils::ByteArray<>> &key ) -> std::shared_ptr<utils::ByteArray<>>;

    /**
     * Decrypts the data with RC4 (unsafe)
     *
     * @param data data to decrypt
     * @param key key for the decryption
     *
     * @return Decrypted result
     */
    export auto decrypt_rc4( const std::shared_ptr<utils::ByteArray<>> &data,
                             const std::shared_ptr<utils::ByteArray<>> &key ) -> std::shared_ptr<utils::ByteArray<>>;
} // namespace vrock::security