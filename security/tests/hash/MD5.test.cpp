import vrock.security.hash;

#include <gtest/gtest.h>

TEST( MD5Test, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Test" );

    EXPECT_EQ( vrock::security::md5( data ).to_hex_string( ), "0cbc6611f5540bd0809a388dc95a615b" );
}