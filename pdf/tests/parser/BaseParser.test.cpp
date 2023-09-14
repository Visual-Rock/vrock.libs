#include <gtest/gtest.h>

import vrock.pdf.BaseParser;

TEST( ParseDouble, BasicAssertions )
{
    vrock::pdf::BaseParser parser( "234.12 123.2134 -1222.111", 7 );
    EXPECT_EQ( parser.parse_double( ), 123.2134 );
    parser.skip_whitespace( );
    EXPECT_EQ( parser.parse_double( ), -1222.111 );
    EXPECT_EQ( parser.parse_double( ), 0.0 );
}

TEST( ParseInt, BasicAssertions )
{
    vrock::pdf::BaseParser parser( "234 123 -1222", 4 );
    EXPECT_EQ( parser.parse_int( ), 123 );
    parser.skip_whitespace( );
    EXPECT_EQ( parser.parse_int( ), -1222 );
    EXPECT_EQ( parser.parse_int( ), 0 );
}

TEST( Is, BasicAssertion )
{
    vrock::pdf::BaseParser parser( "\t<1-.", 0 );
    EXPECT_EQ( parser.is_whitespace( parser.get_char( ) ), true );
    parser.advance( );
    EXPECT_EQ( parser.is_whitespace( parser.get_char( ) ), false );
    EXPECT_EQ( parser.is_delimiter( parser.get_char( ) ), true );
    parser.advance( );
    EXPECT_EQ( parser.is_delimiter( parser.get_char( ) ), false );
    EXPECT_EQ( parser.is_digit( parser.get_char( ) ), true );
    parser.advance( );
    EXPECT_EQ( parser.is_digit( parser.get_char( ) ), false );
    EXPECT_EQ( parser.is_digit_plus_minus( parser.get_char( ) ), true );
    parser.advance( );
    EXPECT_EQ( parser.is_digit_plus_minus( parser.get_char( ) ), false );
    EXPECT_EQ( parser.is_digit_plus_minus_dot( parser.get_char( ) ), true );
}

TEST( Skip, BasicAssertion )
{
    {
        vrock::pdf::BaseParser parser( "  \t%jshfdkhdf\r\n    \n%kjshd\nkhsfa", 0 );
        EXPECT_EQ( parser.get_char( ), ' ' );
        parser.skip_whitespace( );
        EXPECT_EQ( parser.get_char( ), '%' );
        parser.skip_comment( );
        EXPECT_EQ( parser.get_char( ), ' ' );
    }
    {
        vrock::pdf::BaseParser parser( "  \t%jshfdkhdf\r\n    \n%kjshd\nkhsfa", 0 );
        EXPECT_EQ( parser.get_char( ), ' ' );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.get_char( ), 'k' );
    }
}

TEST( IsKeyword, BasicAssertion )
{
    vrock::pdf::BaseParser parser( "xref obj", 0 );
    EXPECT_EQ( parser.is_keyword( "xref" ), true );
    parser.skip_whitespace( );
    EXPECT_EQ( parser.is_keyword( "obj" ), true );
}