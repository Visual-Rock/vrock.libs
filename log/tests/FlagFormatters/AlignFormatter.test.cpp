#include <gtest/gtest.h>

#include "vrock/log/FlagFormatters/AlignFormatter.hpp"
#include "vrock/log/FlagFormatters/GeneralFormatters.hpp"

using namespace vrock::log;

TEST( AlignFormatterTest, LeftAlignFormatterTest )
{
    LeftAlignFormatter formatter( 20, true );
    formatter.set_formatter( std::make_unique<MessageFormatter>( ) );
    {
        Message msg( "Hello, World!" );
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "Hello, World!       " );
    }
    {
        Message msg( "Hello, World! Hello, World!" );
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "Hello, World! Hello," );
    }
}

TEST( AlignFormatterTest, RightAlignFormatterTest )
{
    RightAlignFormatter formatter( 20, true );
    formatter.set_formatter( std::make_unique<MessageFormatter>( ) );
    {
        Message msg( "Hello, World!" );
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "       Hello, World!" );
    }
    {
        Message msg( "Hello, World! Hello, World!" );
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "Hello, World! Hello," );
    }
}

TEST( AlignFormatterTest, CenterAlignFormatterTest )
{
    CenterAlignFormatter formatter( 20, true );
    formatter.set_formatter( std::make_unique<MessageFormatter>( ) );
    {
        Message msg( "Hello, World!" );
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "   Hello, World!    " );
    }
    {
        Message msg( "Hello, World! Hello, World!" );
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "Hello, World! Hello," );
    }
}

// TEST( AlignFormatterTest, CenterAlignFormatterTest )
// {
//     CenterAlignFormatter formatter( 10, true );
//     Message msg( "Hello, World!" );
//     buffer_t buffer;
//
//     formatter.format( msg, buffer );
//     std::string result = buffer.to_string( );
//
//     EXPECT_EQ( result, "Hello, Wor" );
// }
//
// TEST( AlignFormatterTest, RightAlignFormatterTest )
// {
//     RightAlignFormatter formatter( 10, true );
//     Message msg( "Hello, World!" );
//     buffer_t buffer;
//
//     formatter.format( msg, buffer );
//     std::string result = buffer.to_string( );
//
//     EXPECT_EQ( result, "Hello, Wor" );
// }
