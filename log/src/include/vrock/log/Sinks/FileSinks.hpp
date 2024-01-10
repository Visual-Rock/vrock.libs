#pragma once

#include "Sink.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>

namespace vrock::log
{
    /**
     * @brief The FileSink class represents a log sink that writes log messages to a file.
     *
     * This class is derived from the Sink base class and overrides the log and flush functions to handle log messages
     * by writing them to a specified file. The log messages are formatted based on the provided pattern.
     */
    class FileSink final : public Sink
    {
    public:
        /**
         * @brief Constructor for the FileSink class.
         *
         * @param path The path to the file where log messages will be written.
         * @param pattern A string_view representing the custom log message pattern.
         *                Defaults to the global pattern if not provided.
         */
        explicit FileSink( const std::filesystem::path &path, std::string_view pattern = get_global_pattern( ) );

        /**
         * @brief Destructor for the FileSink class.
         */
        ~FileSink( ) override;

        /**
         * @brief Handles log messages by writing them to the specified file.
         *
         * @param message The log message to be processed.
         */
        void log( const Message &message ) override;

        /**
         * @brief Flushes any buffered content to the file.
         */
        void flush( ) override;

    private:
        std::ofstream file_; ///< The ofstream object for writing log messages to the file.
    };

    class DailyFileSink final : public Sink
    {
    public:
        DailyFileSink( std::string_view path, std::chrono::hours hour, std::chrono::minutes minutes,
                       std::uint16_t max_files = 0, bool truncate = false, std::string_view = get_global_pattern( ) );

        void log( const Message &message ) override;
        void flush( ) override;

    private:
        std::string_view filename_format;
        formatter_collection_t filename_formatters_;
        std::chrono::time_point<std::chrono::system_clock> time;
        std::ofstream file_;
        std::chrono::hours hour_;
        std::chrono::minutes minutes_;
        std::uint16_t max_files_;
        bool truncate_;
    };
} // namespace vrock::log