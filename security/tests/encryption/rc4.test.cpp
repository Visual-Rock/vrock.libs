#include <vrock/security.hpp>
#include <vrock/utils.hpp>

using namespace vrock::security;
using namespace vrock::utils;

#include <gtest/gtest.h>

TEST( RC4EncryptTest, BasicAssertion )
{
    std::vector<std::uint8_t> data( 10, '\0' );
    std::vector<std::uint8_t> key( 10, '\0' );

    {
        auto enc = encrypt_rc4( data, key );
        EXPECT_EQ( to_hex_string( enc ), "de188941a3375d3a8a06" );
        EXPECT_EQ( decrypt_rc4( enc, key ), data );
    }

    {
        auto enc = encrypt_rc4( to_string( data ), to_string( key ) );
        EXPECT_EQ( to_hex_string( enc ), "de188941a3375d3a8a06" );
        EXPECT_EQ( decrypt_rc4( enc, to_string( key ) ), to_string( data ) );
    }
}