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

    /**
     * @class FileRingBuffer
     * @brief A ring buffer that stores a fixed number of file paths.
     *
     * The FileRingBuffer class allows pushing file paths into a ring buffer
     * with a fixed size. When the buffer is full, new file paths replace the
     * oldest ones in a circular manner, and the replaced files are deleted on disk.
     */
    class FileRingBuffer
    {
    public:
        /**
         * @brief Constructor for FileRingBuffer.
         * @param size The maximum number of file paths to be stored in the buffer.
         */
        explicit FileRingBuffer( std::size_t size );

        /**
         * @brief Pushes a file path into the ring buffer.
         *
         * If the buffer is full, the oldest file is replaced, and the corresponding
         * file on disk is deleted.
         *
         * @param path The file path to be pushed into the buffer.
         */
        auto push_file( std::string path ) -> void;

        /**
         * @brief Retrieves the currently active file path in the ring buffer.
         *
         * The get_current_file method returns the file path of the currently active
         * file in the ring buffer. This is the file that will receive new log entries.
         *
         * @return The file path of the currently active file.
         */
        auto get_current_file( ) -> std::string;

    private:
        std::size_t size_;                     ///< Maximum size of the ring buffer.
        std::size_t index_ = 0;                ///< Current index in the circular buffer.
        std::vector<std::string> files_ = { }; ///< Vector to store file paths in the ring buffer.
    };

    /**
     * @class DailyFileSink
     * @brief A logging sink that writes log messages to a new file each day.
     *
     * The DailyFileSink class is a logging sink that creates a new log file each day
     * at the specified time. It uses a FileRingBuffer to manage a limited number of
     * log files, replacing the oldest ones when the maximum limit is reached.
     */
    class DailyFileSink final : public Sink
    {
    public:
        /**
         * @brief Constructor for DailyFileSink.
         * @param path The formattable base path for the log files.
         * @param hour The hour at which to roll over to a new log file.
         * @param minutes The minute at which to roll over to a new log file.
         * @param max_files The maximum number of log files to retain. Default is 5.
         * @param truncate If true, truncate the log file on opening. Default is false.
         * @param pattern A string_view representing the custom log message pattern.
         *                Defaults to the global pattern if not provided.
         */
        DailyFileSink( std::string_view path, std::chrono::hours hour, std::chrono::minutes minutes,
                       std::uint16_t max_files = 5, bool truncate = false,
                       std::string_view pattern = get_global_pattern( ) );

        /**
         * @brief Writes a log message to the current log file.
         * @param message The log message to be written.
         */
        void log( const Message &message ) override;

        /**
         * @brief Flushes any buffered log messages to the current log file.
         */
        void flush( ) override;

    private:
        std::string_view filename_format;                        ///< Format for generating log file names.
        formatter_collection_t filename_formatters_;             ///< Formatters for constructing log file names.
        std::chrono::time_point<std::chrono::system_clock> time; ///< Time of the last rollover.
        std::ofstream file_;                                     ///< Current log file stream.
        FileRingBuffer files_;                                   ///< Ring buffer for managing log files.
        std::chrono::hours hour_;                                ///< Hour at which to roll over to a new log file.
        std::chrono::minutes minutes_;                           ///< Minute at which to roll over to a new log file.
        bool truncate_;                                          ///< If true, truncate the log file on opening.
    };

    /**
     * @class SizeFileSink
     * @brief A logging sink that creates new log files based on size constraints.
     *
     * The SizeFileSink class is a logging sink that creates new log files when the
     * current file size surpasses a specified maximum. It uses a FileRingBuffer to
     * manage a limited number of log files, replacing the oldest ones when the maximum
     * limit is reached.
     */
    class SizeFileSink final : public Sink
    {
    public:
        /**
         * @brief Constructor for SizeFileSink.
         * @param path The base path for the log files.
         * @param max_file_size The maximum size (in bytes) for each log file. Default is 4 MB.
         * @param max_files The maximum number of log files to retain. Default is 5.
         * @param truncate If true, truncate the log file on opening. Default is false.
         * @param pattern A string_view representing the custom log message pattern.
         *                Defaults to the global pattern if not provided.
         */
        explicit SizeFileSink( std::string_view path, std::size_t max_file_size = 4 * 1024 * 1024,
                               std::uint16_t max_files = 5, bool truncate = false,
                               std::string_view pattern = get_global_pattern( ) );

        /**
         * @brief Writes a log message to the current log file.
         * @param message The log message to be written.
         */
        void log( const Message &message ) override;

        /**
         * @brief Flushes any buffered log messages to the current log file.
         */
        void flush( ) override;

    private:
        std::string_view filename_format;            ///< Format for generating log file names.
        formatter_collection_t filename_formatters_; ///< Formatters for constructing log file names.
        std::size_t max_file_size_;                  ///< Max file size that should not be surpassed
        std::ofstream file_;                         ///< Current log file stream.
        FileRingBuffer files_;                       ///< Ring buffer for managing log files.
        bool truncate_;                              ///< If true, truncate the log file on opening.
    };
} // namespace vrock::log