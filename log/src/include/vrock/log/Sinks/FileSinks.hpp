#pragma once

#include "Sink.hpp"

#include <filesystem>
#include <fstream>

namespace vrock::log
{
    class FileSink final : public Sink
    {
    public:
        FileSink( std::filesystem::path path );
        ~FileSink( ) override;

        void log( const Message &message ) override;
        void flush( ) override;

    private:
        std::ofstream file_;
    };
} // namespace vrock::log