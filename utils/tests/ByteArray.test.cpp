import vrock.utils.ByteArray;

#include <gtest/gtest.h>

using namespace vrock::utils;

TEST( ByteArrayConstructor, BasicAssertion )
{
    {
        ByteArray ba = ByteArray( );
        EXPECT_EQ( ba.size( ), 0 );
        EXPECT_EQ( ba.data( ), nullptr );
        EXPECT_EQ( ba.to_string( ), "" );
        EXPECT_EQ( ba.to_hex_string( ), "" );
    }

    {
        ByteArray ba = ByteArray( 5 );

        EXPECT_EQ( ba.size( ), 5 );
        EXPECT_NE( ba.data( ), nullptr );
        EXPECT_EQ( ba.to_hex_string( ), "0000000000" );
    }

    {
        ByteArray ba = ByteArray( "Test" );

        EXPECT_EQ( ba.size( ), 4 );
        EXPECT_NE( ba.data( ), nullptr );
        EXPECT_EQ( ba.to_string( ), "Test" );
        EXPECT_EQ( ba.to_hex_string( ), "54657374" );
    }
}

TEST(ByteArrayAppend, BasicAssertion)
{
    ByteArray ba = ByteArray( "Test" );
    ByteArray ba1 = ByteArray( "Te" );
    ByteArray ba2 = ByteArray( "st" );

    EXPECT_NO_FATAL_FAILURE( ba1.append( ba2 ) );
    EXPECT_EQ( ba, ba1 );

    ByteArray arr1 = ByteArray( );
    ByteArray arr2 = ByteArray( );

    EXPECT_NO_FATAL_FAILURE( arr1.append( arr2 ) );
}

TEST( ByteArrayReserve, BasicAssertion )
{
    ByteArray ba = ByteArray( "Te" );

    EXPECT_NO_FATAL_FAILURE( ba.reserve( 4 ));
    EXPECT_NO_FATAL_FAILURE( ba[ 2 ] = 's' );
    EXPECT_NO_FATAL_FAILURE( ba[ 3 ] = 't' );

    EXPECT_EQ( ba.to_string( ), "Test" );
    EXPECT_NO_FATAL_FAILURE( ba.reserve( 2 ) );
    EXPECT_EQ( ba.to_string( ), "Te" );
}

TEST( ByteArraySubArray, BasicAssertion )
{
    ByteArray ba = ByteArray( "Test" );

    EXPECT_EQ( ba.subarr( 2 ).to_string( ), "st" );
    EXPECT_EQ( ba.subarr( 2, 1 ).to_string( ), "s" );

}