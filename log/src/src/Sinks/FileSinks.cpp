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

    auto check_file_time( std::chrono::time_point<std::chrono::system_clock> time, std::chrono::hours hours,
                          std::chrono::minutes minutes ) -> bool
    {
    }

    DailyFileSink::DailyFileSink( std::string_view path, std::chrono::hours hour, std::chrono::minutes minutes,
                                  std::uint16_t max_files, bool truncate, std::string_view pattern )
        : Sink( pattern, false ), hour_( hour ), minutes_( minutes ), max_files_( max_files ), truncate_( truncate )
    {
        if ( hour.count( ) > 23 || minutes.count( ) > 59 )
            throw std::runtime_error( "rotation time has to be smaller than 23 hours and 59 minutes" );
        const auto tp = std::chrono::system_clock::now( );
        time = tp - std::chrono::days( 1 ) + std::chrono::hours( hour_ ) + std::chrono::minutes( minutes );
    }

} // namespace vrock::log