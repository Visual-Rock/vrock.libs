#pragma once

#include <string>

#include "vrock/log/FlagFormatters/PatternFormatter.hpp"
#include "vrock/log/Message.hpp"

namespace vrock::log
{
    /**
     * @brief The Sink class represents a base class for log sinks, which handle the output of log messages.
     *
     * Log sinks can be configured with various options, including ANSI color usage and log message patterns.
     * Derived classes must implement the virtual log function to define how log messages are handled.
     */
    class Sink
    {
    public:
        /**
         * @brief Constructor for the Sink class with both ANSI color usage and a custom log message pattern.
         *
         * @param use_ansi A boolean indicating whether ANSI colors should be used.
         * @param pattern A string_view representing the custom log message pattern.
         */
        Sink( std::string_view pattern, bool use_ansi );

        /**
         * @brief Virtual destructor for the Sink class.
         */
        virtual ~Sink( ) = default;

        /**
         * @brief Sets a custom log message pattern for the sink.
         *
         * @param pattern A string_view representing the custom log message pattern.
         */
        auto set_pattern( std::string_view pattern ) -> void;

        /**
         * @brief Virtual function to handle log messages.
         *
         * Derived classes must implement this function to define how log messages are handled.
         *
         * @param message The log message to be processed.
         */
        virtual void log( const Message &message ) = 0;

        /**
         * @brief Pure virtual method to flush any buffered log entries.
         *
         * This method should be implemented by concrete classes derived from Sink.
         * It is responsible for ensuring that any buffered log entries are written
         * or processed to their intended destination.
         */
        virtual void flush( ) = 0;

    protected:
        /**
         * @brief Generates a formatted log message based on the sink's configured pattern.
         *
         * @param msg The log message to be formatted.
         * @return A string representing the formatted log message.
         */
        auto write( const Message &msg ) -> std::string;

        bool use_ansi_colors_ = false;
        std::string_view pattern_;
        formatter_collection_t compiled_pattern;
    };

    /**
     * @brief The Sinkable concept represents types that can be used as log sinks and inherit from the Sink base class.
     *
     * This concept checks whether a type is a derived class of the Sink class, indicating it can be used as a log sink.
     *
     * @tparam T The type to be checked against the Sink concept.
     */
    template <typename T>
    concept Sinkable = std::is_base_of_v<Sink, T>;
} // namespace vrock::log