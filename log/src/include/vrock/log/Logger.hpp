#pragma once

#include "LogLevel.hpp"
#include "LogMessage.hpp"
#include "Message.hpp"
#include "Sinks/Sink.hpp"
#include "ThreadingPolicy.hpp"

#include <format>
#include <memory>
#include <utility>
#include <vector>

namespace vrock::log
{
    template <typename ThreadingPolicy = SingleThreadedPolicy>
    class Logger
    {
    public:
        Logger( ) = default;
        explicit Logger( std::string name, const LogLevel level,
                         const std::string_view pattern = get_global_pattern( ) )
            : level_( level ), name_( std::move( name ) ), pattern_( pattern )
        {
        }

        template <Sinkable SinkType>
        auto add_sink( std::shared_ptr<SinkType> sink, const bool set_pattern = true ) -> void
        {
            if ( set_pattern )
                sink->set_pattern( pattern_ );
            sinks_.push_back( sink );
        }

        auto set_pattern( std::string_view pattern, bool change_on_sinks = true ) -> void
        {
            pattern_ = pattern;
            if ( change_on_sinks )
            {
                for ( const auto &sink : sinks_ )
                    sink->set_pattern( pattern );
            }
        }

        auto set_level( const LogLevel &level ) -> void
        {
            level_ = level;
        }

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

                const typename ThreadingPolicy::lock lock( mutex_ );
                for ( const auto &sink : sinks_ )
                    sink->log( msg );
            }
        }

        template <typename... Args>
        auto trace( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Trace, args... );
        }

        template <typename... Args>
        auto debug( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Debug, args... );
        }

        template <typename... Args>
        auto info( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Info, args... );
        }

        template <typename... Args>
        auto warn( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Warn, args... );
        }

        template <typename... Args>
        auto error( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, LogLevel::Error, args... );
        }

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
        auto flush( ) const -> void;

    private:
        LogLevel level_ = LogLevel::Info;
        std::string name_;
        std::string_view pattern_;
        std::vector<std::shared_ptr<Sink>> sinks_;
        typename ThreadingPolicy::mutex_t mutex_;
    };

    using MTLogger = Logger<MultiThreadedPolicy>;
} // namespace vrock::log