#include "vrock/log/LoggerStorage.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include <vrock/log/Sinks/ConsoleSinks.hpp>

namespace vrock::log
{
    std::unordered_map<std::string_view, std::shared_ptr<Logger>> logger_;

    auto make_logger( std::string_view name, const LogLevel level, const bool multi_threaded,
                      const std::string_view pattern ) -> logger_t
    {
        logger_[ name ] = std::shared_ptr<Logger>( new Logger( name, level, multi_threaded, pattern ) );
        return logger_[ name ];
    }

    auto get_logger( const std::string_view name ) -> std::shared_ptr<Logger>
    {
        if ( logger_.contains( name ) )
            return logger_[ name ];
        auto logger = make_logger( name );
        if ( add_standard_out_to_default )
            logger->add_sink( std::make_shared<StandardOutSink>( ) );
        return logger;
    }

} // namespace vrock::log