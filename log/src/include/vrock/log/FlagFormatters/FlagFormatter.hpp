#pragma once

#include <string>

#include "../Message.hpp"

namespace vrock::log
{
    using buffer_t = std::string;

    class FlagFormatter
    {
    public:
        FlagFormatter( ) = default;
        virtual ~FlagFormatter( ) = default;

        virtual void format( const Message &msg, buffer_t &buffer ) = 0;
    };
} // namespace vrock::log