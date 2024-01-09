#pragma once

#include "Sink.hpp"

#include <filesystem>
#include <fstream>

namespace vrock::log
{
    class FileSink final : public Sink
    {
    public:
        explicit FileSink( const std::filesystem::path &path, std::string_view pattern = get_global_pattern( ) );
        ~FileSink( ) override;

        void log( const Message &message ) override;
        void flush( ) override;

    private:
        std::ofstream file_;
    };
} // namespace vrock::log