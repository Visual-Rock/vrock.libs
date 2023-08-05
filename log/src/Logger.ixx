module;

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>

export module vrock.log.Logger;

namespace vrock::log
{
    /// @brief Loglevel (equivalent to spdlog level's)
    export enum class LogLevel
    {
        Trace = spdlog::level::trace,
        Debug = spdlog::level::debug,
        Info = spdlog::level::info,
        Warning = spdlog::level::warn,
        Error = spdlog::level::err,
        Critical = spdlog::level::critical,
        Off = spdlog::level::off
    };

    export class LoggerConfig
    {
    public:
        /// @brief creates a new instance of LoggerConfig that can be used to initialize a new Logger
        /// @param name name of the Logger
        explicit LoggerConfig( std::string name ) : name( std::move( name ) ), cfg( { } )
        {
        }

        /// @brief gets all spdlog sinks
        /// @return vector containing all sinks
        auto get_config( ) const -> std::vector<spdlog::sink_ptr>
        {
            return cfg;
        }

        /// @brief set log-level
        /// @param lvl log level
        /// @return return this LoggerConfig
        auto set_log_level( LogLevel lvl ) -> LoggerConfig &
        {
            level = lvl;
            return *this;
        }

        /// @brief add a colored console sink
        /// @return return this LoggerConfig
        auto add_console_colored( ) -> LoggerConfig &
        {
            cfg.push_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
            return *this;
        }

        /// @brief add a colored error sink
        /// @return return this LoggerConfig
        auto add_colored_error( ) -> LoggerConfig &
        {
            cfg.push_back( std::make_shared<spdlog::sinks::stderr_color_sink_mt>( ) );
            return *this;
        }

        /// @brief add a daily file sink
        /// @param file filename of the logfile
        /// @param amount amount of files to keep
        /// @return return this LoggerConfig
        auto add_daily_file( const std::string &file, uint16_t amount = 5 ) -> LoggerConfig &
        {
            cfg.push_back( std::make_shared<spdlog::sinks::daily_file_sink_mt>( file, 23, 59, false, amount ) );
            return *this;
        }

        /// @brief add rotating file sink
        /// @param file filename of the logfiles
        /// @param file_size filesize (default 4MB)
        /// @param amount amount of files
        /// @return return this LoggerConfig
        auto add_rotating_file( const std::string &file, size_t file_size = 4194304, uint16_t amount = 5 )
            -> LoggerConfig &
        {
            cfg.push_back( std::make_shared<spdlog::sinks::rotating_file_sink_mt>( file, file_size, amount ) );
            return *this;
        }

    public:
        /// @brief name of the Logger
        std::string name;

        LogLevel level = LogLevel::Trace;

    private:
        std::vector<spdlog::sink_ptr> cfg;
    };

    static std::unordered_map<std::string, spdlog::logger> loggers = { };

    /// @brief creates a logger with a given config
    /// @param cfg config for the logger
    /// @return shared pointer containing the logger
    export auto create_logger( const LoggerConfig &cfg ) -> spdlog::logger
    {
        const std::string &name = cfg.name;
        if ( loggers.find( name ) != loggers.end( ) )
            loggers.erase( name ); // already exists so it gets overwritten
        auto sinks = cfg.get_config( );
        if ( sinks.empty( ) )
            sinks.push_back( std::make_shared<spdlog::sinks::null_sink_mt>( ) );
        loggers.insert( { name, spdlog::logger( name, sinks.begin( ), sinks.end( ) ) } );
        auto logger = loggers.find( name )->second;
        logger.set_level( (spdlog::level::level_enum)cfg.level );
        logger.flush_on( (spdlog::level::level_enum)cfg.level );
        return logger;
    }

    /// @brief creates a logger with a given name
    /// @param n name of the logger
    /// @return shared pointer containing the logger
    export auto create_logger( const std::string &n ) -> spdlog::logger
    {
        if ( loggers.find( n ) != loggers.end( ) )
            loggers.erase( n ); // already exists so it gets overwritten
        std::vector<spdlog::sink_ptr> sinks;
        sinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
        loggers.insert( { n, spdlog::logger( n, sinks.begin( ), sinks.end( ) ) } );
        auto logger = loggers.find( n )->second;
        logger.set_level( spdlog::level::info );
        logger.flush_on( spdlog::level::info );
        return logger;
    }

    /// @brief gets the logger with the given name. if the logger is not found it creates one.
    /// @param name name of the logger
    /// @return shared pointer containing the logger
    export auto get_logger( const std::string &name ) -> spdlog::logger
    {
        if ( loggers.find( name ) != loggers.end( ) )
            return loggers.find( name )->second;
        return create_logger( name );
    }
} // namespace vrock::log