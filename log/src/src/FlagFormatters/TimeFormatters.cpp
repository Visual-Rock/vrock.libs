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
        buffer.append( std::format( "{0:%S}", time_point_cast<seconds>( msg.time ) ) );
    }

    void MillisecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", time_point_cast<milliseconds>( msg.time ) ) );
    }

    void MicrosecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", time_point_cast<microseconds>( msg.time ) ) );
    }

    void NanosecondFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%S}", time_point_cast<nanoseconds>( msg.time ) ) );
    }

    void AmPmFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%p}", msg.time ) );
    }

    void Time12Formatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%I:%M:%S %p}", time_point_cast<seconds>( msg.time ) ) );
    }

    void Time24Formatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%R}", msg.time ) );
    }

    void ISO8601TimeFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%T}", time_point_cast<seconds>( msg.time ) ) );
    }

    void ISO8601TimezoneFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{0:%z}", time_point_cast<seconds>( msg.time ) ) );
    }

    void TimeSinceEpochFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::format( "{}", duration_cast<seconds>( msg.time.time_since_epoch( ) ).count( ) ) );
    }
} // namespace vrock::log