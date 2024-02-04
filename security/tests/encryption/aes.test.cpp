#include <vrock/security.hpp>
#include <vrock/utils.hpp>

using namespace vrock::security;
using namespace vrock::utils;

#include <gtest/gtest.h>

TEST( AESTest, AESGCMTest )
{
    std::vector<std::uint8_t> data( 16, '\0' );
    std::vector<std::uint8_t> key( 32, '\0' );
    std::vector<std::uint8_t> iv( 12, '\0' );
    std::vector<std::uint8_t> aad( 16, '\0' );

    {
        auto encrypted = encrypt_aes_gcm( data, key, iv, aad );
        EXPECT_EQ( to_hex_string( encrypted ), "cea7403d4d606b6e074ec5d3baf39d18ae9b1771dba9cf62b39be017940330b4" );
        EXPECT_EQ( decrypt_aes_gcm( encrypted, key, iv, aad ), data );
    }

    {
        auto encrypted = encrypt_aes_gcm( to_string( data ), to_string( key ), to_string( iv ), to_string( aad ) );
        EXPECT_EQ( to_hex_string( encrypted ), "cea7403d4d606b6e074ec5d3baf39d18ae9b1771dba9cf62b39be017940330b4" );
        EXPECT_EQ( decrypt_aes_gcm( encrypted, to_string( key ), to_string( iv ), to_string( aad ) ),
                   to_string( data ) );
    }
}

TEST( AESTest, AESECBTest )
{
    std::vector<std::uint8_t> data( 16, '\0' );
    std::vector<std::uint8_t> key( 32, '\0' );

    {
        auto encrypted = encrypt_aes_ecb( data, key );
        EXPECT_EQ( to_hex_string( encrypted ), "dc95c078a2408989ad48a21492842087" );
        EXPECT_EQ( decrypt_aes_ecb( encrypted, key ), data );
    }

    {
        auto encrypted = encrypt_aes_ecb( to_string( data ), to_string( key ) );
        EXPECT_EQ( to_hex_string( encrypted ), "dc95c078a2408989ad48a21492842087" );
        EXPECT_EQ( decrypt_aes_ecb( encrypted, to_string( key ) ), to_string( data ) );
    }
}

TEST( AESTest, AESCBCTest )
{
    std::vector<std::uint8_t> data( 16, '\0' );
    std::vector<std::uint8_t> key( 32, '\0' );
    std::vector<std::uint8_t> iv( 16, '\0' );

    {
        auto encrypted = encrypt_aes_cbc( data, key, iv );
        EXPECT_EQ( to_hex_string( encrypted ), "dc95c078a2408989ad48a21492842087" );
        EXPECT_EQ( decrypt_aes_cbc( encrypted, key, iv ), data );
    }

    {
        auto encrypted = encrypt_aes_cbc( to_string( data ), to_string( key ), to_string( iv ) );
        EXPECT_EQ( to_hex_string( encrypted ), "dc95c078a2408989ad48a21492842087" );
        EXPECT_EQ( decrypt_aes_cbc( encrypted, to_string( key ), to_string( iv ) ), to_string( data ) );
    }
}