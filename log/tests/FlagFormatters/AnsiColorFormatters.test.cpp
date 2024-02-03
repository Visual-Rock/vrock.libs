#include <gtest/gtest.h>

#include "vrock/log/FlagFormatters/AnsiFormatters.hpp"

#include <vrock/log/AnsiColors.hpp>

using namespace vrock::log;

TEST( AnsiColorFormatterTest, AnsiColorFormatterTest )
{
    AnsiColorFormatter formatter( 'r', false );

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "\033[31m" );
}

TEST( AnsiColorFormatterTest, AnsiResetFormatterTest )
{
    AnsiResetFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "\033[m" );
}

TEST( AnsiColorFormatterTest, AnsiLogLevelColorFormatterTest )
{
    AnsiLogLevelColorFormatter formatter;

    Message msg( "" );
    msg.level = LogLevel::Error;

    {
        ErrorColor = LogLevelColor( AnsiColor::Red, false, false );
        buffer_t buffer;

        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "\033[31m" );
    }

    {
        ErrorColor = LogLevelColor( AnsiColor::Red, true, false );
        buffer_t buffer;

        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "\033[31m\033[1m" );
    }

    {
        ErrorColor = LogLevelColor( AnsiColor::Red, true, true );
        buffer_t buffer;

        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, "\033[31m\033[1m\033[4m" );
    }
}

TEST( AnsiColorFormatterTest, AnsiBoldFormatterTest )
{
    AnsiBoldFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "\033[1m" );
}

TEST( AnsiColorFormatterTest, AnsiUnderlineFormatterTest )
{
    AnsiUnderlineFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "\033[4m" );
}