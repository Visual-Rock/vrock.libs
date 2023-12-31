#include "vrock/log/Logger.hpp"

namespace vrock::log
{
    Logger::Logger( std::string name, const LogLevel level ) : level_( level ), name_( std::move( name ) )
    {
    }

    auto Logger::set_level( const LogLevel &level ) -> void
    {
        level_ = level;
    }
} // namespace vrock::log