#include "vrock/log/Sinks/FileSinks.hpp"

#include <iostream>

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

    using namespace std::chrono;

    auto check_file_time( time_point<system_clock> time, hours hours, minutes minutes ) -> bool
    {
    }

    DailyFileSink::DailyFileSink( std::string_view path, hours hour, minutes min, std::uint16_t max_files,
                                  bool truncate, std::string_view pattern )
        : Sink( pattern, false ), hour_( hour ), minutes_( min ), max_files_( max_files ), truncate_( truncate )
    {
        if ( hour.count( ) > 23 || min.count( ) > 59 )
            throw std::runtime_error( "rotation time has to be smaller than 23 hours and 59 minutes" );
        const auto tp = time_point_cast<days>( system_clock::now( ) );
        time = tp + hour_ + minutes_;
        filename_formatters_ = compile_pattern( path, false );

        Message msg{ };
        msg.time = time_point_cast<nanoseconds>( system_clock::now( ) );

        std::string file_name;
        for ( const auto &formatter : filename_formatters_ )
            formatter->format( msg, file_name );
        file_ = std::ofstream( file_name, truncate_ ? std::ios_base::trunc : std::ios_base::app );
    }

    void DailyFileSink::log( const Message &message )
    {
        // change file
        if ( message.time > time )
        {
            Message msg{ };
            msg.time = time;

            const auto tp = time_point_cast<days>( message.time );
            time = tp + hour_ + minutes_ + days( 1 );

            std::string file_name;
            for ( const auto &formatter : filename_formatters_ )
                formatter->format( msg, file_name );

            if ( file_.is_open( ) )
                file_.close( );
            file_ = std::ofstream( file_name, truncate_ ? std::ios::trunc : std::ios_base::app );
        }
        file_ << write( message ) << std::endl;
    }

    void DailyFileSink::flush( )
    {
        file_ << std::flush;
    }

} // namespace vrock::log