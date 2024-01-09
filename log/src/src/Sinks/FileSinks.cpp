#include "vrock/log/Sinks/FileSinks.hpp"

namespace vrock::log
{
    FileSink::FileSink( const std::filesystem::path &path, std::string_view pattern ) : Sink( pattern, false )
    {
        file_ = std::ofstream( path );
    }

    FileSink::~FileSink( )
    {
        file_.close( );
    }

    void FileSink::log( const Message &message )
    {
        file_ << write( message ) << std::endl;
    }

    void FileSink::flush( )
    {
        file_ << std::flush;
    }
} // namespace vrock::log