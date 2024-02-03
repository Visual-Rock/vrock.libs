#include "vrock/log/Sinks/Sink.hpp"

namespace vrock::log
{
    Sink::Sink( std::string_view pattern, bool use_ansi ) : use_ansi_colors_( use_ansi )
    {
        set_pattern( pattern );
    }

    auto Sink::set_pattern( std::string_view pattern ) -> void
    {
        pattern_ = pattern;
        compiled_pattern = compile_pattern( pattern_, use_ansi_colors_ );
    }

    auto Sink::write( const Message &msg ) -> std::string
    {
        std::string formatted;
        for ( const auto &i : compiled_pattern )
            i->format( msg, formatted );
        return formatted;
    }
} // namespace vrock::log