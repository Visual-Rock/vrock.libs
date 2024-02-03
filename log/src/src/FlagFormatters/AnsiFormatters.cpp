#include "vrock/log/FlagFormatters/AnsiFormatters.hpp"

#include <utility>

#include "vrock/log/AnsiColors.hpp"

namespace vrock::log
{
    AnsiColorFormatter::AnsiColorFormatter( char color, bool background )
    {
        std::uint8_t c = std::to_underlying( flag_to_color( color ) );
        if ( background )
            c += 10;
        code_ = std::format( "\033[{}m", c );
    }

    void AnsiColorFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( code_ );
    }

    void AnsiResetFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( "\033[m" );
    }

    void AnsiLogLevelColorFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( get_log_level_color( msg.level ).command_sequence );
    }

    void AnsiBoldFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( "\033[1m" );
    }
    
    void AnsiUnderlineFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( "\033[4m" );
    }
} // namespace vrock::log