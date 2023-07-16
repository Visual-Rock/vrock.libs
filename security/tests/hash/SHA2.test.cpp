import vrock.security.hash;

#include <gtest/gtest.h>

TEST( SHA224Test, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Test" );

    EXPECT_EQ( vrock::security::sha224( data ).to_hex_string( ),
               "3606346815fd4d491a92649905a40da025d8cf15f095136b19f37923" );
}

TEST( SHA256Test, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Test" );

    EXPECT_EQ( vrock::security::sha256( data ).to_hex_string( ),
               "532eaabd9574880dbf76b9b8cc00832c20a6ec113d682299550d7a6e0f345e25" );
}

TEST( SHA384Test, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Test" );

    EXPECT_EQ( vrock::security::sha384( data ).to_hex_string( ),
               "7b8f4654076b80eb963911f19cfad1aaf4285ed48e826f6cde1b01a79aa73fadb5446e667fc4f90417782c91270540f3" );
}

TEST( SHA512Test, BasicAssertion )
{
    auto data = vrock::utils::ByteArray( "Test" );

    EXPECT_EQ( vrock::security::sha512( data ).to_hex_string( ),
               "c6ee9e33cf5c6715a1d148fd73f7318884b41adcb916021e2bc0e800a5c5dd97f5142178f6ae88c8fdd98e1afb0ce4c8d2c54b5"
               "f37b30b7da1997bb33b0b8a31" );
}