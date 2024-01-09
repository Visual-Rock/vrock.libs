#include "vrock/log/Sinks/FileSinks.hpp"

namespace vrock::log
{
    FileSink::FileSink( std::filesystem::path path ) : Sink( "%v", false )
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