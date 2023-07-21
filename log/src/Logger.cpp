module;

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <unordered_map>
#include <vector>

module vrock.log.Logger;

namespace vrock::log
{
    LoggerConfig::LoggerConfig( std::string n ) : name( std::move( n ) ), cfg( { } )
    {
    }

    auto LoggerConfig::get_config( ) const -> std::vector<spdlog::sink_ptr>
    {
        return cfg;
    }

    auto LoggerConfig::set_log_level( LogLevel lvl ) -> LoggerConfig &
    {
        level = lvl;
        return *this;
    }

    auto LoggerConfig::add_console_colored( ) -> LoggerConfig &
    {
        cfg.push_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
        return *this;
    }

    auto LoggerConfig::add_colored_error( ) -> LoggerConfig &
    {
        cfg.push_back( std::make_shared<spdlog::sinks::stderr_color_sink_mt>( ) );
        return *this;
    }

    auto LoggerConfig::add_daily_file( const std::string &file, uint16_t amount ) -> LoggerConfig &
    {
        cfg.push_back( std::make_shared<spdlog::sinks::daily_file_sink_mt>( file, 23, 59, false, amount ) );
        return *this;
    }

    auto LoggerConfig::add_rotating_file( const std::string &file, size_t file_size, uint16_t amount ) -> LoggerConfig &
    {
        cfg.push_back( std::make_shared<spdlog::sinks::rotating_file_sink_mt>( file, file_size, amount ) );
        return *this;
    }

    static std::unordered_map<std::string, spdlog::logger> loggers = { };

    auto create_logger( const LoggerConfig &cfg ) -> spdlog::logger &
    {
        const std::string &name = cfg.name;
        if ( loggers.find( name ) != loggers.end( ) )
            loggers.erase( name ); // already exists so it gets overwritten
        auto sinks = cfg.get_config( );
        if ( sinks.empty( ) )
            sinks.push_back( std::make_shared<spdlog::sinks::null_sink_mt>( ) );
        loggers.insert( { name, spdlog::logger( name, sinks.begin( ), sinks.end( ) ) } );
        auto &logger = loggers.find( name )->second;
        logger.set_level( (spdlog::level::level_enum)cfg.level );
        logger.flush_on( (spdlog::level::level_enum)cfg.level );
        return logger;
    }

    auto create_logger( const std::string &n ) -> spdlog::logger &
    {
        if ( loggers.find( n ) != loggers.end( ) )
            loggers.erase( n ); // already exists so it gets overwritten
        std::vector<spdlog::sink_ptr> sinks;
        sinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
        loggers.insert( { n, spdlog::logger( n, sinks.begin( ), sinks.end( ) ) } );
        auto &logger = loggers.find( n )->second;
        logger.set_level( spdlog::level::info );
        logger.flush_on( spdlog::level::info );
        return logger;
    }

    auto get_logger( const std::string &name ) -> spdlog::logger &
    {
        if ( loggers.find( name ) != loggers.end( ) )
            return loggers.find( name )->second;
        return create_logger( name );
    }
} // namespace vrock::log