#pragma once

#include "Sink.hpp"

namespace vrock::log
{
    class ConsoleSink final : public Sink
    {
    public:
        ConsoleSink( ) : Sink( true, "%v" )
        {
        }

        explicit ConsoleSink( const std::string_view pattern ) : Sink( true, pattern )
        {
        }
        ~ConsoleSink( ) override = default;

        auto log( const Message &message ) -> void override
        {
            fmt::println( "{}", write( message ) );
        }
    };
} // namespace vrock::log