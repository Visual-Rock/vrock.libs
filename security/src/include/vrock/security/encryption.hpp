#pragma once

#include "typedefs.hpp"

namespace vrock::security
{
    enum class Padding
    {
        NoPadding,
        ZerosPadding,
        PkcsPadding,
        W3CPadding,
        OneAndZerosPadding
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
    auto encrypt_aes_gcm( byte_span_t data, byte_span_t key, byte_span_t iv, byte_span_t authentication_data )
        -> return_t;

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
    auto decrypt_aes_gcm( byte_span_t data, byte_span_t key, byte_span_t iv, byte_span_t authentication_data )
        -> return_t;

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
    auto encrypt_aes_gcm( string_view_t data, string_view_t key, string_view_t iv, string_view_t authentication_data )
        -> return_string_t;

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
    auto decrypt_aes_gcm( string_view_t data, string_view_t key, string_view_t iv, string_view_t authentication_data )
        -> return_string_t;

    /**
     * Encrypt the data with AES in ECB mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param padding Padding schema
     *
     * @return Encrypted result
     */
    auto encrypt_aes_ecb( byte_span_t data, byte_span_t key, Padding padding = Padding::NoPadding ) -> return_t;

    /**
     * Decrypts the data with AES in ECB mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param padding Padding schema
     *
     * @return Decrypted result
     */
    auto decrypt_aes_ecb( byte_span_t data, byte_span_t key, Padding padding = Padding::NoPadding ) -> return_t;

    /**
     * Encrypt the data with AES in ECB mode
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     * @param padding Padding schema
     *
     * @return Encrypted result
     */
    auto encrypt_aes_ecb( string_view_t data, string_view_t key, Padding padding = Padding::NoPadding )
        -> return_string_t;

    /**
     * Decrypts the data with AES in ECB mode
     *
     * @param data data to decrypt
     * @param key key for the decryption
     * @param padding Padding schema
     *
     * @return Decrypted result
     */
    auto decrypt_aes_ecb( string_view_t data, string_view_t key, Padding padding = Padding::NoPadding )
        -> return_string_t;

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
    auto encrypt_aes_cbc( byte_span_t data, byte_span_t key, byte_span_t iv, Padding padding = Padding::NoPadding )
        -> return_t;

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
    auto decrypt_aes_cbc( byte_span_t data, byte_span_t key, byte_span_t iv, Padding padding = Padding::NoPadding )
        -> return_t;

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
    auto encrypt_aes_cbc( string_view_t data, string_view_t key, string_view_t iv,
                          Padding padding = Padding::NoPadding ) -> return_string_t;

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
    auto decrypt_aes_cbc( string_view_t data, string_view_t key, string_view_t iv,
                          Padding padding = Padding::NoPadding ) -> return_string_t;

    /**
     * Encrypt the data with RC4 (unsafe)
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     *
     * @return Encrypted result
     */
    auto encrypt_rc4( byte_span_t data, byte_span_t key ) -> return_t;

    /**
     * Decrypts the data with RC4 (unsafe)
     *
     * @param data data to decrypt
     * @param key key for the decryption
     *
     * @return Decrypted result
     */
    auto decrypt_rc4( byte_span_t data, byte_span_t key ) -> return_t;

    /**
     * Encrypt the data with RC4 (unsafe)
     *
     * @param data data to encrypt
     * @param key key for the encrypt
     *
     * @return Encrypted result
     */
    auto encrypt_rc4( string_view_t data, string_view_t key ) -> return_string_t;

    /**
     * Decrypts the data with RC4 (unsafe)
     *
     * @param data data to decrypt
     * @param key key for the decryption
     *
     * @return Decrypted result
     */
    auto decrypt_rc4( string_view_t data, string_view_t key ) -> return_string_t;
} // namespace vrock::security