#pragma once

#include <source_location>
#include <string_view>

namespace vrock::log
{
    class LogMessage
    {
    public:
        template <typename T>
        constexpr LogMessage( T msg, const std::source_location loc = std::source_location::current( ) )
            : message{ msg }, source_location( loc )
        {
        }

        std::string_view message;
        std::source_location source_location;
    };
} // namespace vrock::log