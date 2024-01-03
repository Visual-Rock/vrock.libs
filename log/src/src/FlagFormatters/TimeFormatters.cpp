#include "vrock/log/FlagFormatters/TimeFormatters.hpp"

#include <array>

using namespace std::chrono;

namespace vrock::log
{
    const std::array<std::string, 7> day_names = { "Sunday",   "Monday", "Tuesday", "Wednesday",
                                                   "Thursday", "Friday", "Saturday" };
    const std::array<std::string, 7> short_day_names = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

    std::array<std::string, 12> month_names = { "January", "February", "March",     "April",   "May",      "June",
                                                "July",    "August",   "September", "October", "November", "December" };
    std::array<std::string, 12> short_month_names = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    void WeekdayNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        weekday d{ floor<days>( msg.time ) };
        buffer.append( day_names[ d.c_encoding( ) ] );
    }

    void WeekdayShortNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        weekday d{ floor<days>( msg.time ) };
        buffer.append( short_day_names[ d.c_encoding( ) ] );
    }

    void MonthNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        auto m = time_point_cast<months>( msg.time ).time_since_epoch( ).count( ) % 12;
        buffer.append( month_names[ m ] );
    }

    void MonthShortNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        auto m = time_point_cast<months>( msg.time ).time_since_epoch( ).count( ) % 12;
        buffer.append( short_month_names[ m ] );
    }
} // namespace vrock::log