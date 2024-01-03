#pragma once

#include <chrono>

#include "FlagFormatter.hpp"

namespace vrock::log
{
    class WeekdayNameFormatter : public FlagFormatter
    {
    public:
        WeekdayNameFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class WeekdayShortNameFormatter : public FlagFormatter
    {
    public:
        WeekdayShortNameFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MonthNameFormatter : public FlagFormatter
    {
    public:
        MonthNameFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MonthShortNameFormatter : public FlagFormatter
    {
    public:
        MonthShortNameFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class DateTimeFormatter : public FlagFormatter
    {
    public:
        DateTimeFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class YearFormatter : public FlagFormatter
    {
    public:
        YearFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class ShortYearFormatter : public FlagFormatter
    {
    public:
        ShortYearFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class ShortDateFormatter : public FlagFormatter
    {
    public:
        ShortDateFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class LocalDateFormatter : public FlagFormatter
    {
    public:
        LocalDateFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MonthFormatter : public FlagFormatter
    {
    public:
        MonthFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class DayFormatter : public FlagFormatter
    {
    public:
        DayFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class Hour24Formatter : public FlagFormatter
    {
    public:
        Hour24Formatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class Hour12Formatter : public FlagFormatter
    {
    public:
        Hour12Formatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MinuteFormatter : public FlagFormatter
    {
    public:
        MinuteFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class SecondFormatter : public FlagFormatter
    {
    public:
        SecondFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MillisecondFormatter : public FlagFormatter
    {
    public:
        MillisecondFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MicrosecondFormatter : public FlagFormatter
    {
    public:
        MicrosecondFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class NanosecondFormatter : public FlagFormatter
    {
    public:
        NanosecondFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class AmPmFormatter : public FlagFormatter
    {
    public:
        AmPmFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class Time12Formatter : public FlagFormatter
    {
    public:
        Time12Formatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class Time24Formatter : public FlagFormatter
    {
    public:
        Time24Formatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class ISO8601TimeFromatter : public FlagFormatter
    {
    public:
        ISO8601TimeFromatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class ISO8601TimezoneFromatter : public FlagFormatter
    {
    public:
        ISO8601TimezoneFromatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };
} // namespace vrock::log