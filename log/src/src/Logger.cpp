#include "vrock/log/Logger.hpp"

namespace vrock::log
{
    template <typename ThreadingPolicy>
    Logger<ThreadingPolicy>::Logger( std::string name, const LogLevel level, const std::string_view pattern )
        : level_( level ), name_( std::move( name ) ), pattern_( pattern )
    {
    }

    template <typename ThreadingPolicy>
    auto Logger<ThreadingPolicy>::set_level( const LogLevel &level ) -> void
    {
        level_ = level;
    }

    template <typename ThreadingPolicy>
    auto Logger<ThreadingPolicy>::set_pattern( std::string_view pattern, bool change_on_sinks ) -> void
    {
        pattern_ = pattern;
        if ( change_on_sinks )
        {
            for ( const auto &sink : sinks_ )
                sink->set_pattern( pattern );
        }
    }

    template <typename ThreadingPolicy>
    auto Logger<ThreadingPolicy>::flush( ) const -> void
    {
        for ( const auto &sink : sinks_ )
            sink->flush( );
    }
} // namespace vrock::log