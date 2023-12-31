#pragma once

#include "ExecutionContext.hpp"
#include "LogLevel.hpp"

#include <chrono>
#include <source_location>

namespace vrock::log
{
    class Message
    {
    public:
        Message( ) = default;
        Message( const std::string_view str ) : message( str )
        {
        }

        LogLevel level;
        std::string_view message;
        std::source_location source_location;
        std::chrono::time_point<std::chrono::system_clock> time;
        ExecutionContext execution_context;
    };
} // namespace vrock::log