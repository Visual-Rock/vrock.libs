import vrock.utils.ByteArray;
import vrock.security.encryption;

#include <gtest/gtest.h>

TEST( AESECBEncryptTest, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Plaintext" );
    auto key = vrock::utils::ByteArray( "KeyKeyKeyKeyKeyK" );

    auto encrypted = vrock::security::encrypt_aes_ecb( data, key, vrock::security::Padding::PKCS_PADDING );
    EXPECT_EQ( encrypted.to_string( ),
               vrock::utils::from_hex_string( "c9aed1347dc38bfe3345cc5b33391487" ).to_string( ) );
    EXPECT_EQ( vrock::security::decrypt_aes_ecb( encrypted, key, vrock::security::Padding::PKCS_PADDING ).to_string( ),
               "Plaintext" );
}

TEST( AESCBCEncryptTest, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Test" );
    auto key = vrock::utils::from_hex_string( "B10851065A82E228EE29CF3A8322DB6A" );
    auto iv = vrock::utils::from_hex_string( "AC0EF343B92D165D8E75703C7B3E0770" );

    auto encrypted = vrock::security::encrypt_aes_cbc( data, key, iv, vrock::security::Padding::PKCS_PADDING );
    EXPECT_EQ( encrypted.to_string( ),
               vrock::utils::from_hex_string( "78f293772a958631d43de02e31b84673" ).to_string( ) );
    EXPECT_EQ(
        vrock::security::decrypt_aes_cbc( encrypted, key, iv, vrock::security::Padding::PKCS_PADDING ).to_string( ),
        "Test" );
}

TEST( AESGCMEncryptTest, BasicAssertion )
{
    auto data = vrock::utils::from_hex_string( "00000000000000000000000000000000" );
    auto key = vrock::utils::from_hex_string( "0000000000000000000000000000000000000000000000000000000000000000" );
    auto iv = vrock::utils::from_hex_string( "000000000000000000000000" );
    auto aad = vrock::utils::from_hex_string( "00000000000000000000000000000000" );

    auto encrypted = vrock::security::encrypt_aes_gcm( data, key, iv, aad );
    EXPECT_EQ( encrypted.to_string( ),
               vrock::utils::from_hex_string( "cea7403d4d606b6e074ec5d3baf39d18ae9b1771dba9cf62b39be017940330b4" )
                   .to_string( ) );
    EXPECT_EQ( vrock::security::decrypt_aes_gcm( encrypted, key, iv, aad ).to_string( ), data.to_string( ) );
}