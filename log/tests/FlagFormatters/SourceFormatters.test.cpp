#include "vrock/log/FlagFormatters/SourceFormatters.hpp"

#include <gtest/gtest.h>

using namespace vrock::log;

TEST( SourceFormatters, SourceFileFormatterTest )
{
    Message msg;

    {
        SourceFileFormatter<true> formatter;
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, msg.source_location.file_name( ) );
    }

    {
        SourceFileFormatter<false> formatter;
        buffer_t buffer;
        formatter.format( msg, buffer );
        EXPECT_EQ( buffer, std::filesystem::path( msg.source_location.file_name( ) ).filename( ) );
    }
}

TEST( SourceFormatters, SourceLineFormatterTest )
{
    Message msg;

    SourceLineFormatter formatter;
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, std::to_string( msg.source_location.line( ) ) );
}

TEST( SourceFormatters, SourceColumnFormatterTest )
{
    Message msg;

    SourceColumnFormatter formatter;
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, std::to_string( msg.source_location.column( ) ) );
}

TEST( SourceFormatters, SourceFunctionFormatterTest )
{
    Message msg;

    SourceFunctionFormatter formatter;
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, msg.source_location.function_name( ) );
}