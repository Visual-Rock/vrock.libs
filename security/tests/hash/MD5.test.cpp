#include <vrock/security.hpp>

#include <gtest/gtest.h>

#include <vrock/utils/SpanHelpers.hpp>

using namespace vrock::utils;
using namespace vrock::security;

TEST( MD5Test, BasicAssertion )
{
    const auto data = "Test";
    EXPECT_EQ( to_hex_string( md5( data ) ), "0cbc6611f5540bd0809a388dc95a615b" );
}