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
} // namespace vrock::log