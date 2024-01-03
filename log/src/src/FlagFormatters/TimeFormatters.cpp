#include "vrock/log/FlagFormatters/TimeFormatters.hpp"

#include <array>

using namespace std::chrono;

namespace vrock::log
{
    void WeekdayNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%A}", msg.time ) );
    }

    void WeekdayShortNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%a}", msg.time ) );
    }

    void MonthNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%B}", msg.time ) );
    }

    void MonthShortNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%b}", msg.time ) );
    }

    void DateTimeFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%c}", msg.time ) );
    }

    void YearFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%Y}", msg.time ) );
    }

    void ShortYearFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%y}", msg.time ) );
    }

    void ShortDateFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%D}", msg.time ) );
    }

    void LocalDateFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%x}", msg.time ) );
    }

    void MonthFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%m}", msg.time ) );
    }

    void DayFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%d}", msg.time ) );
    }

    void Hour24Formatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%H}", msg.time ) );
    }

    void Hour12Formatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%I}", msg.time ) );
    }

    void MinuteFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%M}", msg.time ) );
    }

    void SecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", std::chrono::time_point_cast<std::chrono::seconds>( msg.time ) ) );
    }

    void MillisecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", std::chrono::time_point_cast<std::chrono::milliseconds>( msg.time ) ) );
    }

    void MicrosecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", std::chrono::time_point_cast<std::chrono::microseconds>( msg.time ) ) );
    }

    void NanosecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", std::chrono::time_point_cast<std::chrono::nanoseconds>( msg.time ) ) );
    }
} // namespace vrock::log