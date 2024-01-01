#pragma once

#include "LogLevel.hpp"
#include "LogMessage.hpp"
#include "Message.hpp"
#include "Sink.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

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
        Logger( std::string name, const LogLevel level );

        template <Sinkable SinkType, typename... Args>
        auto add_sink( Args &&...params ) -> void
        {
            sinks_.push_back( std::make_shared<SinkType>( std::forward<Args>( params )... ) );
        }

        constexpr auto set_pattern( std::string_view pattern ) -> void;

        auto set_level( const LogLevel &level ) -> void;

        template <typename... Args>
        auto log( const LogMessage &message, const LogLevel level, Args &&...args ) -> void
        {
            if ( includes_level( level_, level ) )
            {
                auto msg = Message( );
                msg.time = std::chrono::system_clock::now( );
                fmt::basic_memory_buffer<char, 250> buf;
                fmt::vformat_to( fmt::appender( buf ), message.message, fmt::make_format_args( args... ) );
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
        auto info( const LogMessage &message, Args &&...args ) -> void
        {
            log( message, vrock::log::LogLevel::Info, args... );
        }

    private:
        template <typename... Args>
        std::string format( const std::string_view str, Args &&...args )
        {
            return std::vformat( str, std::make_format_args( args... ) );
        }

        LogLevel level_;
        std::string name_;
        std::vector<std::shared_ptr<Sink>> sinks_;
    };

    constexpr auto Logger::set_pattern( std::string_view pattern ) -> void
    {
    }
} // namespace vrock::log