#pragma once

#include "LogLevel.hpp"
#include "LogMessage.hpp"
#include "Sinks/Sink.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace vrock::log
{
    /**
     * @brief The Logger class provides a flexible logging framework.
     *
     * The Logger class allows for easy configuration of log levels, log message patterns, and sinks
     * to control where log messages are sent.
     */
    class Logger
    {
    public:
        /**
         * @brief Default constructor for the Logger class.
         */
        Logger( ) = default;

        /**
         * @brief Constructor for the Logger class with specified parameters.
         *
         * @param name The name of the logger.
         * @param level The initial log level for the logger.
         * @param multi_threaded Flag indicating whether the logger is multi-threaded.
         * @param pattern The log message pattern (defaults to the global pattern).
         */
        explicit Logger( std::string name, const LogLevel level, const bool multi_threaded = false,
                         const std::string_view pattern = get_global_pattern( ) )
            : level_( level ), multi_threaded_( multi_threaded ), name_( std::move( name ) ), pattern_( pattern )
        {
        }

        /**
         * @brief Adds a sink to the logger.
         *
         * @tparam SinkType The type of the sink to be added.
         * @param sink The sink to be added.
         * @param set_pattern Flag indicating whether to set the logger pattern on the sink.
         */
        template <Sinkable SinkType>
        auto add_sink( std::shared_ptr<SinkType> sink, const bool set_pattern = true ) -> void
        {
            if ( set_pattern )
                sink->set_pattern( pattern_ );
            sinks_.push_back( sink );
        }

        /**
         * @brief Sets the log message pattern for the logger.
         *
         * @param pattern The new log message pattern.
         * @param change_on_sinks Flag indicating whether to change the pattern on existing sinks.
         */
        auto set_pattern( std::string_view pattern, bool change_on_sinks = true ) -> void
        {
            pattern_ = pattern;
            if ( change_on_sinks )
            {
                for ( const auto &sink : sinks_ )
                    sink->set_pattern( pattern );
            }
        }

        /**
         * @brief Sets the log level for the logger.
         *
         * @param level The new log level.
         */
        auto set_level( const LogLevel &level ) -> void
        {
            level_ = level;
        }

        /**
         * @brief Logs a formatted log message with the specified log level.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param level The log level for the log message.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto log( const LogMessage &message, const LogLevel level, Args &&...args ) -> void
        {
            if ( includes_level( level_, level ) )
            {
                auto msg = Message( );
                msg.time = std::chrono::system_clock::now( );
                std::string buf;
                buf.reserve( 250 );
                std::vformat_to( std::back_inserter( buf ), message.message, std::make_format_args( args... ) );
                msg.message = std::string_view( buf.data( ), buf.size( ) );
                msg.logger_name = name_;
                msg.execution_context = this_execution_context;
                msg.level = level;
                msg.source_location = message.source_location;

                if ( multi_threaded_ )
                {
                    std::lock_guard _lock( mutex_ );
                    for ( const auto &sink : sinks_ )
                        sink->log( msg );
                }
                else
                {
                    for ( const auto &sink : sinks_ )
                        sink->log( msg );
                }
            }
        }

        /**
         * @brief Logs a trace-level message.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto trace( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Trace, args... );
        }

        /**
         * @brief Logs a debug-level message.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto debug( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Debug, args... );
        }

        /**
         * @brief Logs an info-level message.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto info( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Info, args... );
        }

        /**
         * @brief Logs a warn-level message.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto warn( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Warn, args... );
        }

        /**
         * @brief Logs an error-level message.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto error( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Error, args... );
        }

        /**
         * @brief Logs a critical-level message.
         *
         * @tparam Args Variadic template for additional arguments used in formatting the log message.
         * @param message The log message structure containing the format string and source location.
         * @param args Additional arguments used in formatting the log message.
         */
        template <typename... Args>
        auto critical( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Critical, args... );
        }

        /**
         * @brief Flushes any buffered log entries in the Logger.
         *
         * The flush method in the Logger class is responsible for ensuring that any
         * buffered log entries are immediately processed or written to their intended
         * destinations. This method can be used to enforce timely flushing of log data.
         */
        auto flush( ) const -> void
        {
            for ( const auto &sink : sinks_ )
                sink->flush( );
        }

    private:
        LogLevel level_ = LogLevel::Info;          /**< The current log level for the logger. */
        const bool multi_threaded_ = false;        /**< Flag indicating whether the logger is multi-threaded. */
        std::mutex mutex_;                         /**< Mutex for thread synchronization in multi-threaded logging. */
        std::string name_;                         /**< The name of the logger. */
        std::string_view pattern_;                 /**< The log message pattern for the logger. */
        std::vector<std::shared_ptr<Sink>> sinks_; /**< Vector of sinks attached to the logger. */
    };
} // namespace vrock::log