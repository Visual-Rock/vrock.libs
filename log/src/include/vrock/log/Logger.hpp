#pragma once

#include "LogLevel.hpp"
#include "LogMessage.hpp"
#include "Message.hpp"
#include "Sinks/Sink.hpp"

#include <format>
#include <memory>
#include <utility>
#include <vector>

namespace vrock::log
{
    class Logger
    {
    public:
        Logger( ) = default;
        Logger( std::string name, LogLevel level );

        template <Sinkable SinkType, typename... Args>
        auto add_sink( Args &&...params ) -> std::shared_ptr<SinkType>
        {
            auto sink = std::make_shared<SinkType>( params... );
            sink->set_pattern( pattern_ );
            sinks_.push_back( sink );
            return sink;
        }

        auto set_pattern( std::string_view pattern, bool change_on_sinks = true ) -> void;

        auto set_level( const LogLevel &level ) -> void;

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

                for ( const auto &sink : sinks_ )
                    sink->log( msg );
            }
        }

        template <typename... Args>
        auto inline trace( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Trace, args... );
        }

        template <typename... Args>
        auto inline debug( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Debug, args... );
        }

        template <typename... Args>
        auto inline info( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Info, args... );
        }

        template <typename... Args>
        auto inline warn( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Warn, args... );
        }

        template <typename... Args>
        auto inline error( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Error, args... );
        }

        template <typename... Args>
        auto inline critical( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Critical, args... );
        }

    private:
        LogLevel level_ = LogLevel::Info;
        std::string name_;
        std::string_view pattern_;
        std::vector<std::shared_ptr<Sink>> sinks_;
    };
} // namespace vrock::log