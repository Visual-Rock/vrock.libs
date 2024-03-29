#include "vrock/log/AnsiColors.hpp"

namespace vrock::log
{
    LogLevel global_log_level_ = LogLevel::Info;

    auto flag_to_color( char f ) -> AnsiColor
    {
        switch ( f )
        {
        case 'k':
            return AnsiColor::Black;
        case 'r':
            return AnsiColor::Red;
        case 'g':
            return AnsiColor::Green;
        case 'y':
            return AnsiColor::Yellow;
        case 'b':
            return AnsiColor::Blue;
        case 'm':
            return AnsiColor::Magenta;
        case 'c':
            return AnsiColor::Cyan;
        case 'w':
            return AnsiColor::White;
        case 'd':
            return AnsiColor::Default;
        default:
            throw std::runtime_error( std::format( "color {} not found or not supported.", f ) );
        }
    }

    auto get_global_log_level( ) -> LogLevel
    {
        return global_log_level_;
    }

    auto set_global_log_level( LogLevel level ) -> void
    {
        global_log_level_ = level;
    }

    auto get_log_level_color( const LogLevel &level ) -> const LogLevelColor &
    {
        static auto default_ = LogLevelColor( AnsiColor::Default );

        switch ( level )
        {
        case LogLevel::Trace:
            return TraceColor;
        case LogLevel::Debug:
            return DebugColor;
        case LogLevel::Info:
            return InfoColor;
        case LogLevel::Warn:
            return WarnColor;
        case LogLevel::Error:
            return ErrorColor;
        case LogLevel::Critical:
            return CriticalColor;
        default:
            return default_;
        }
    }
} // namespace vrock::log