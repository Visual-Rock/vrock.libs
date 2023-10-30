#include <vrock/security.hpp>

#include <gtest/gtest.h>

TEST( RC4EncryptTest, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Plaintext" );
    auto key = vrock::utils::ByteArray( "Key" );

    auto enc = vrock::security::encrypt_rc4( data, key );
    EXPECT_EQ( enc.to_string( ), vrock::utils::from_hex_string( "BBF316E8D940AF0AD3" ).to_string( ) );
    EXPECT_EQ( vrock::security::decrypt_rc4( enc, key ).to_string( ), "Plaintext" );
}