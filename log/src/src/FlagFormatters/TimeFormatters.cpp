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
} // namespace vrock::log