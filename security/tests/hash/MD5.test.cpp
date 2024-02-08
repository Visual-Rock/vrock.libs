#include <vrock/security.hpp>

#include <gtest/gtest.h>

#include <vrock/utils/SpanHelpers.hpp>

using namespace vrock::utils;
using namespace vrock::security;

TEST( MD5Test, BasicAssertion )
{
    std::vector<uint8_t> data = { 'T', 'e', 's', 't' };

    {
        auto md5_hash = md5( data );
        EXPECT_EQ( to_hex_string( md5_hash ), "0cbc6611f5540bd0809a388dc95a615b" );
    }

    {
        auto md5_hash = md5( to_string( data ) );
        EXPECT_EQ( to_hex_string( md5_hash ), "0cbc6611f5540bd0809a388dc95a615b" );
    }
}