#include "vrock/log/FlagFormatters/GeneralFormatters.hpp"

#include <gtest/gtest.h>

using namespace vrock::log;

TEST( GeneralFormatterTest, UserCharacterFormatterTest )
{
    UserCharactersFormatter formatter( "Hello, World!" );

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "Hello, World!" );
}

TEST( GeneralFormatterTest, LevelFormatterTest )
{
    LevelFormatter formatter;

    Message msg( "" );
    msg.level = LogLevel::Info;
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "info" );
}

TEST( GeneralFormatterTest, LoggerNameFormatterTest )
{
    LoggerNameFormatter formatter;

    Message msg( "" );
    msg.logger_name = "vrock::log";
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "vrock::log" );
}

TEST( GeneralFormatterTest, MessageFormatterTest )
{
    MessageFormatter formatter;

    Message msg( "Hello, World!" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "Hello, World!" );
}

TEST( GeneralFormatterTest, ThreadIDFormatterTest )
{
    ThreadIDFormatter formatter;

    Message msg( "" );
    msg.execution_context = ExecutionContext( );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, msg.execution_context.thread_id );
}

TEST( GeneralFormatterTest, ProcessIDFormatterTest )
{
    ProcessIDFormatter formatter;

    Message msg( "" );
    msg.execution_context = ExecutionContext( );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, std::to_string( msg.execution_context.process_id ) );
}