#pragma once

#include <cstdint>
#include <string>

namespace vrock::log
{
    /**
     * @brief Enumeration representing different log levels.
     *
     * Log levels are used to categorize log messages based on their severity.
     */
    enum class LogLevel : std::uint8_t
    {
        Trace = ( 1 << 0 ),    ///< Trace-level log messages.
        Debug = ( 1 << 1 ),    ///< Debug-level log messages.
        Info = ( 1 << 2 ),     ///< Info-level log messages.
        Warn = ( 1 << 3 ),     ///< Warn-level log messages.
        Error = ( 1 << 4 ),    ///< Error-level log messages.
        Critical = ( 1 << 5 ), ///< Critical-level log messages.
        None = ( 1 << 6 )      ///< Log level indicating no messages should be logged.
    };

    /**
     * @brief Bitwise OR operator for LogLevel enumeration.
     *
     * @param lhs The left-hand side LogLevel.
     * @param rhs The right-hand side LogLevel.
     * @return The result of the OR operation.
     */
    constexpr auto operator|( const LogLevel &lhs, const LogLevel &rhs ) -> LogLevel
    {
        return static_cast<LogLevel>( static_cast<std::underlying_type_t<LogLevel>>( lhs ) |
                                      static_cast<std::underlying_type_t<LogLevel>>( rhs ) );
    }

    /**
     * @brief Bitwise AND operator for LogLevel enumeration.
     *
     * @param lhs The left-hand side LogLevel.
     * @param rhs The right-hand side LogLevel.
     * @return The result of the AND operation.
     */
    constexpr auto operator&( const LogLevel &lhs, const LogLevel &rhs ) -> LogLevel
    {
        return static_cast<LogLevel>( static_cast<std::underlying_type_t<LogLevel>>( lhs ) &
                                      static_cast<std::underlying_type_t<LogLevel>>( rhs ) );
    }

    /**
     * @brief Checks if a base log level includes another log level.
     *
     * @param base The base LogLevel to check.
     * @param should_include The LogLevel to check if it is included in the base.
     * @return True if should_include is included in base, false otherwise.
     */
    inline auto includes_level( const LogLevel &base, const LogLevel &should_include ) -> bool
    {
        return static_cast<std::underlying_type_t<LogLevel>>( base ) <=
               static_cast<std::underlying_type_t<LogLevel>>( should_include );
    }

    /**
     * @brief Converts LogLevel to a string representation.
     *
     * @param level The LogLevel to convert.
     * @return A string_view representing the LogLevel.
     */
    constexpr auto to_string( const LogLevel &level ) -> std::string_view
    {
        switch ( level )
        {
        case LogLevel::Debug:
            return "debug";
        case LogLevel::Info:
            return "info";
        case LogLevel::Warn:
            return "warn";
        case LogLevel::Error:
            return "error";
        case LogLevel::Critical:
            return "critical";
        case LogLevel::Trace:
            return "trace";
        default:
            return "unknown";
        }
    }
} // namespace vrock::log