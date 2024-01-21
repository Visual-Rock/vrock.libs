#include <vrock/security.hpp>

#include <gtest/gtest.h>

#include <vrock/utils/SpanHelpers.hpp>

using namespace vrock::utils;
using namespace vrock::security;

TEST( SHA224Test, BasicAssertion )
{
    const auto data = "Test";

    EXPECT_EQ( to_hex_string( sha224( data ) ), "3606346815fd4d491a92649905a40da025d8cf15f095136b19f37923" );
}

TEST( SHA256Test, BasicAssertion )
{
    const auto data = "Test";

    EXPECT_EQ( to_hex_string( sha256( data ) ), "532eaabd9574880dbf76b9b8cc00832c20a6ec113d682299550d7a6e0f345e25" );
}

TEST( SHA384Test, BasicAssertion )
{
    const auto data = "Test";

    EXPECT_EQ( to_hex_string( sha384( data ) ),
               "7b8f4654076b80eb963911f19cfad1aaf4285ed48e826f6cde1b01a79aa73fadb5446e667fc4f90417782c91270540f3" );
}

TEST( SHA512Test, BasicAssertion )
{
    const auto data = "Test";

    EXPECT_EQ( to_hex_string( sha512( data ) ),
               "c6ee9e33cf5c6715a1d148fd73f7318884b41adcb916021e2bc0e800a5c5dd97f5142178f6ae88c8fdd98e1afb0ce4c8d2c54b5"
               "f37b30b7da1997bb33b0b8a31" );
}