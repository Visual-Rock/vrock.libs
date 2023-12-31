#pragma once

#include "Message.hpp"

namespace vrock::log
{
    class Sink
    {
    public:
        explicit Sink( const bool use_ansi, const std::string_view pattern )
            : use_ansi_colors_( use_ansi ), pattern_( pattern )
        {
        }
        virtual ~Sink( ) = default;

        virtual void log( const Message &message ) = 0;

    protected:
        auto write( const Message &msg ) -> std::string
        {
            return std::string( msg.message );
        }

        bool use_ansi_colors_;
        std::string_view pattern_;
    };

    template <typename T>
    concept Sinkable = std::is_base_of_v<Sink, T>;
} // namespace vrock::log