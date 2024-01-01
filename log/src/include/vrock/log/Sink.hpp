#pragma once

#include <string>

#include "Message.hpp"
#include "PatternFormatter.hpp"

namespace vrock::log
{
    class Sink
    {
    public:
        explicit Sink( const bool use_ansi, const std::string_view pattern )
            : use_ansi_colors_( use_ansi ), pattern_( pattern )
        {
            compiled_pattern = compile_pattern( pattern );
        }
        virtual ~Sink( ) = default;

        virtual void log( const Message &message ) = 0;

    protected:
        auto write( const Message &msg ) -> std::string
        {
            std::string formatted;
            for ( const auto &i : compiled_pattern )
                i->format( msg, formatted );
            return formatted;
        }

        bool use_ansi_colors_;
        std::string_view pattern_;
        formatter_collection_t compiled_pattern;
    };

    template <typename T>
    concept Sinkable = std::is_base_of_v<Sink, T>;
} // namespace vrock::log