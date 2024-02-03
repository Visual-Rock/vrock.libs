#pragma once

#include "Sink.hpp"

namespace vrock::log
{
    /**
     * @brief The StandardOutSink class represents a log sink that outputs log messages to standard output.
     *
     * This class is derived from the Sink base class and overrides the log function to handle log messages
     * by printing them to the standard output.
     */
    class StandardOutSink final : public Sink
    {
    public:
        /**
         * @brief Constructor for the StandardOutSink class.
         *
         * @param pattern A string_view representing the custom log message pattern.
         * @param use_ansi A boolean indicating whether ANSI colors should be used.
         */
        explicit StandardOutSink( std::string_view pattern = get_global_pattern( ), bool use_ansi = true );

        /**
         * @brief Destructor for the StandardOutSink class.
         */
        ~StandardOutSink( ) override = default;

        /**
         * @brief Handles log messages by printing them to standard output.
         *
         * @param message The log message to be processed.
         */
        auto log( const Message &message ) -> void override;

        /**
         * @brief Overrides the flush method to flush std::cout.
         */
        auto flush( ) -> void override;
    };

    /**
     * @brief The StandardErrSink class represents a log sink that outputs log messages to standard error.
     *
     * This class is derived from the Sink base class and overrides the log function to handle log messages
     * by printing them to the standard error. It includes a level filter, printing messages with the specified
     * LogLevel or higher to standard error.
     */
    class StandardErrSink final : public Sink
    {
    public:
        /**
         * @brief Constructor for the StandardErrSink class.
         *
         * @param pattern A string_view representing the custom log message pattern.
         * @param level The minimum log level for messages to be printed to standard error.
         * @param use_ansi A boolean indicating whether ANSI colors should be used.
         */
        explicit StandardErrSink( std::string_view pattern = get_global_pattern( ), LogLevel level = LogLevel::Error,
                                  bool use_ansi = false );

        /**
         * @brief Destructor for the StandardErrSink class.
         */
        ~StandardErrSink( ) override = default;

        /**
         * @brief Handles log messages by printing them to standard error if they meet the specified LogLevel filter.
         *
         * @param message The log message to be processed.
         */
        auto log( const Message &message ) -> void override;

        /**
         * @brief Overrides the flush method to flush std::cerr.
         */
        auto flush( ) -> void override;

    private:
        LogLevel level_;
    };
} // namespace vrock::log