#pragma once

#include <cstdint>
#include <string>

namespace vrock::log
{
    enum class LogLevel : std::uint8_t
    {
        All = ( 1 << 0 ),
        Debug = ( 1 << 1 ),
        Info = ( 1 << 2 ),
        Warning = ( 1 << 3 ),
        Error = ( 1 << 4 ),
        Critical = ( 1 << 5 ),
        None = ( 1 << 6 )
    };

    constexpr auto operator|( const LogLevel &lhs, const LogLevel &rhs ) -> LogLevel
    {
        return static_cast<LogLevel>( static_cast<std::underlying_type_t<LogLevel>>( lhs ) |
                                      static_cast<std::underlying_type_t<LogLevel>>( rhs ) );
    }

    constexpr auto operator&( const LogLevel &lhs, const LogLevel &rhs ) -> LogLevel
    {
        return static_cast<LogLevel>( static_cast<std::underlying_type_t<LogLevel>>( lhs ) &
                                      static_cast<std::underlying_type_t<LogLevel>>( rhs ) );
    }

    inline auto includes_level( const LogLevel &base, const LogLevel &should_include ) -> bool
    {
        return static_cast<std::underlying_type_t<LogLevel>>( base ) <=
               static_cast<std::underlying_type_t<LogLevel>>( should_include );
    }

    constexpr auto to_string( const LogLevel &level ) -> std::string_view
    {
        switch ( level )
        {
        case LogLevel::Debug:
            return "debug";
        case LogLevel::Info:
            return "info";
        case LogLevel::Warning:
            return "warning";
        case LogLevel::Error:
            return "error";
        case LogLevel::Critical:
            return "critical";
        case LogLevel::All:
            return "all";
        default:
            return "unknown";
        }
    }
} // namespace vrock::log