#include "vrock/log/FlagFormatters/TimeFormatters.hpp"

#include <iostream>

#include <gtest/gtest.h>

using namespace vrock::log;

TEST( TimeFormattersTest, WeekdayNameFormatterTest )
{
    WeekdayNameFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "Thursday" );
}

TEST( TimeFormattersTest, WeekdayShortNameFormatterTest )
{
    WeekdayShortNameFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "Thu" );
}

TEST( TimeFormattersTest, MonthNameFormatterTest )
{
    MonthNameFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "January" );
}

TEST( TimeFormattersTest, MonthShortNameFormatterTest )
{
    MonthShortNameFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "Jan" );
}

TEST( TimeFormattersTest, DateTimeFormatterTest )
{
    DateTimeFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    // EXPECT_EQ( buffer, "Thu Jan  1 00:00:00 1970" );
}

TEST( TimeFormattersTest, YearFormatterTest )
{
    YearFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "1970" );
}

TEST( TimeFormattersTest, ShortYearFormatterTest )
{
    ShortYearFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "70" );
}

TEST( TimeFormattersTest, ShortDateFormatterTest )
{
    ShortDateFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "01/01/70" );
}

TEST( TimeFormattersTest, LocalDateFormatterTest )
{
    LocalDateFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "01/01/70" );
}

TEST( TimeFormattersTest, MonthFormatterTest )
{
    MonthFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "01" );
}

TEST( TimeFormattersTest, DayFormatterTest )
{
    DayFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "01" );
}

TEST( TimeFormattersTest, Hour24FormatterTest )
{
    Hour24Formatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00" );
}

TEST( TimeFormattersTest, Hour12FormatterTest )
{
    Hour12Formatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "12" );
}

TEST( TimeFormattersTest, MinuteFormatterTest )
{
    MinuteFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00" );
}

TEST( TimeFormattersTest, SecondFormatterTest )
{
    SecondFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00" );
}

TEST( TimeFormattersTest, MillisecondFormatterTest )
{
    MillisecondFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00.000" );
}

TEST( TimeFormattersTest, MicrosecondFormatterTest )
{
    MicrosecondFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00.000000" );
}

TEST( TimeFormattersTest, NanosecondFormatterTest )
{
    NanosecondFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00.000000000" );
}

TEST( TimeFormattersTest, AmPmFormatterTest )
{
    AmPmFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "AM" );
}

TEST( TimeFormattersTest, Time12FormatterTest )
{
    Time12Formatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "12:00:00 AM" );
}

TEST( TimeFormattersTest, Time24FormatterTest )
{
    Time24Formatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00:00" );
}

TEST( TimeFormattersTest, ISO8601TimeFormatterTest )
{
    ISO8601TimeFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "00:00:00" );
}

TEST( TimeFormattersTest, ISO8601TimezoneFormatterTest )
{
    ISO8601TimezoneFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "+0000" );
}

TEST( TimeFormattersTest, TimeSinceEpochFormatterTest )
{
    TimeSinceEpochFormatter formatter;

    Message msg( "" );
    buffer_t buffer;
    formatter.format( msg, buffer );
    EXPECT_EQ( buffer, "0" );
}