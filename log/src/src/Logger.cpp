#include "vrock/log/Logger.hpp"

namespace vrock::log
{
    Logger::Logger( std::string name, LogLevel level ) : level_( level ), name_( std::move( name ) )
    {
    }

    auto Logger::set_level( const LogLevel &level ) -> void
    {
        level_ = level;
    }

    auto Logger::set_pattern( std::string_view pattern, bool change_on_sinks ) -> void
    {
        pattern_ = pattern;
        if ( change_on_sinks )
        {
            for ( const auto &sink : sinks_ )
                sink->set_pattern( pattern );
        }
    }
} // namespace vrock::log