#include "vrock/log/AnsiColors.hpp"

namespace vrock::log
{
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

    constexpr LogLevelColor::LogLevelColor( AnsiColor color, bool bold, bool underlined, AnsiColor background )
        : color( color ), bold( bold ), underlined( underlined ), background( background )
    {
        if ( background != AnsiColor::Default )
            command_sequence.append( std::format( "\033[{}m", std::underlying_type_t<AnsiColor>( background ) + 10 ) );
        if ( color != AnsiColor::Default )
            command_sequence.append( std::format( "\033[{}m", std::underlying_type_t<AnsiColor>( color ) ) );
        if ( bold )
            command_sequence.append( "\033[1m" );
        if ( underlined )
            command_sequence.append( "\033[4m" );
    }

    auto get_loglevel_color( const LogLevel &level ) -> const LogLevelColor &
    {
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
            return LogLevelColor( AnsiColor::Default );
        }
    }
} // namespace vrock::log