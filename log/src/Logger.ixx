module;

#include <spdlog/spdlog.h>
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
        LoggerConfig( std::string name );

        /// @brief gets all spdlog sinks
        /// @return vector containing all sinks
        auto get_config( ) const -> std::vector<spdlog::sink_ptr>;

        /// @brief set log-level
        /// @param lvl log level
        /// @return return this LoggerConfig
        auto set_log_level( LogLevel lvl ) -> LoggerConfig&;

        /// @brief add a colored console sink
        /// @return return this LoggerConfig
        auto add_console_colored( ) -> LoggerConfig&;

        /// @brief add a colored error sink
        /// @return return this LoggerConfig
        auto add_colored_error( ) -> LoggerConfig&;

        /// @brief add a daily file sink
        /// @param file filename of the logfile
        /// @param amount amount of files to keep
        /// @return return this LoggerConfig
        auto add_daily_file( const std::string &file, uint16_t amount = 5 ) -> LoggerConfig&;

        /// @brief add rotating file sink
        /// @param file filename of the logfiles
        /// @param file_size filesize (default 4MB)
        /// @param amount amount of files
        /// @return return this LoggerConfig
        auto add_rotating_file( const std::string &file, size_t file_size = 4194304, uint16_t amount = 5 )
            -> LoggerConfig&;

    public:
        /// @brief name of the Logger
        std::string name;

        LogLevel level = LogLevel::Trace;

    private:
        std::vector<spdlog::sink_ptr> cfg;
    };

    /// @brief creates a logger with a given config
    /// @param cfg config for the logger
    /// @return shared pointer containing the logger
    export auto create_logger( const LoggerConfig &cfg ) -> spdlog::logger&;

    /// @brief creates a logger with a given name
    /// @param n name of the logger
    /// @return shared pointer containing the logger
    export auto create_logger( const std::string &n ) -> spdlog::logger&;

    /// @brief gets the logger with the given name. if the logger is not found it creates one.
    /// @param name name of the logger
    /// @return shared pointer containing the logger
    export auto get_logger( const std::string &name ) -> spdlog::logger&;
}