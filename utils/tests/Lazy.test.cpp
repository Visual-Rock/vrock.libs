#include <vrock/utils.hpp>

#include <gtest/gtest.h>

using namespace vrock::utils;

TEST( LazyBasicTest, BasicAssertion )
{
    Lazy<int> lazy( [] { return 42; } );

    EXPECT_EQ( lazy.is_loaded( ), false );

    EXPECT_EQ( *lazy, 42 );
    EXPECT_EQ( lazy( ), 42 );
    EXPECT_EQ( lazy.get( ), 42 );

    EXPECT_EQ( lazy.is_loaded( ), true );
}

TEST( LazyAssignTest, BasicAssertion )
{
    Lazy<int> lazy( [] { return 42; } );
    auto lazy2 = lazy;

    EXPECT_EQ( *lazy, 42 );

    lazy = 56;

    EXPECT_EQ( *lazy, 56 );
    EXPECT_EQ( *lazy2, 42 );

    lazy = lazy2;

    EXPECT_EQ( *lazy, 42 );

    *lazy = 32;

    EXPECT_EQ( *lazy, 32 );
}