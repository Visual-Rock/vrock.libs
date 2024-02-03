#pragma once

#include "FlagFormatter.hpp"

#include <filesystem>

namespace vrock::log
{
    /**
     * @brief A formatter class for formatting the source file name in log messages.
     *
     * This class is templated based on whether the full path should be included.
     *
     * @tparam FullPath If true, the full path is included; otherwise, only the file name is included.
     */
    template <bool FullPath>
    class SourceFileFormatter : public FlagFormatter
    {
    public:
        /**
         * @brief Default constructor for the SourceFileFormatter class.
         */
        SourceFileFormatter( ) = default;

        /**
         * @brief Formats the source file information in the log message.
         *
         * @param msg The log message to be formatted.
         * @param buffer The buffer to store the formatted result.
         */
        void format( const Message &msg, buffer_t &buffer ) override;
    };

    /**
     * @brief A formatter class for formatting the source line number in log messages.
     */
    class SourceLineFormatter : public FlagFormatter
    {
    public:
        /**
         * @brief Default constructor for the SourceLineFormatter class.
         */
        SourceLineFormatter( ) = default;

        /**
         * @brief Formats the source line number in the log message.
         *
         * @param msg The log message to be formatted.
         * @param buffer The buffer to store the formatted result.
         */
        void format( const Message &msg, buffer_t &buffer ) override;
    };

    /**
     * @brief A formatter class for formatting the source column number in log messages.
     */
    class SourceColumnFormatter : public FlagFormatter
    {
    public:
        /**
         * @brief Default constructor for the SourceColumnFormatter class.
         */
        SourceColumnFormatter( ) = default;

        /**
         * @brief Formats the source column number in the log message.
         *
         * @param msg The log message to be formatted.
         * @param buffer The buffer to store the formatted result.
         */
        void format( const Message &msg, buffer_t &buffer ) override;
    };

    /**
     * @brief A formatter class for formatting the source function name in log messages.
     */
    class SourceFunctionFormatter : public FlagFormatter
    {
    public:
        /**
         * @brief Default constructor for the SourceFunctionFormatter class.
         */
        SourceFunctionFormatter( ) = default;

        /**
         * @brief Formats the source function name in the log message.
         *
         * @param msg The log message to be formatted.
         * @param buffer The buffer to store the formatted result.
         */
        void format( const Message &msg, buffer_t &buffer ) override;
    };

} // namespace vrock::log