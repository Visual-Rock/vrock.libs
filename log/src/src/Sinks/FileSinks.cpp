#include "vrock/log/Sinks/FileSinks.hpp"

#include <iostream>
#include <utility>

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

    FileRingBuffer::FileRingBuffer( std::size_t size ) : size_( size )
    {
        files_.resize( size_ );
    }

    auto FileRingBuffer::push_file( std::string path ) -> void
    {
        const auto value = files_[ index_ ];
        files_[ index_ ] = std::move( path );

        if ( !value.empty( ) )
            std::filesystem::remove( value );

        index_ = ( index_ + 1 ) % size_;
    }

    auto FileRingBuffer::get_current_file( ) -> std::string
    {
        return files_[ ( size_ + index_ - 1 ) % size_ ];
    }

    DailyFileSink::DailyFileSink( std::string_view path, hours hour, minutes min, std::uint16_t max_files,
                                  bool truncate, std::string_view pattern )
        : Sink( pattern, false ), files_( max_files ), hour_( hour ), minutes_( min ), truncate_( truncate )
    {
        if ( hour.count( ) > 23 || min.count( ) > 59 )
            throw std::runtime_error( "rotation time has to be smaller than 23 hours and 59 minutes" );
        const auto tp = time_point_cast<days>( system_clock::now( ) );
        time = tp + hour_ + minutes_;
        filename_formatters_ = compile_file_pattern( path );

        Message msg{ };
        msg.time = time_point_cast<nanoseconds>( system_clock::now( ) );

        std::string file_name;
        for ( const auto &formatter : filename_formatters_ )
            formatter->format( msg, file_name );
        file_ = std::ofstream( file_name, truncate_ ? std::ios_base::trunc : std::ios_base::app );
        files_.push_file( file_name );
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
            files_.push_file( file_name );
        }
        file_ << write( message ) << std::endl;
    }

    void DailyFileSink::flush( )
    {
        file_ << std::flush;
    }

    SizeFileSink::SizeFileSink( std::string_view path, std::size_t max_file_size, std::uint16_t max_files,
                                bool truncate, std::string_view pattern )
        : Sink( pattern, false ), filename_format( path ), max_file_size_( max_file_size ), files_( max_files ),
          truncate_( truncate )
    {
        if ( max_file_size < 1024 )
            throw std::runtime_error( "rotation size can not be smaller than 1024" );

        filename_formatters_ = compile_file_pattern( path );
        Message msg{ };
        msg.time = time_point_cast<nanoseconds>( system_clock::now( ) );

        std::string file_name;
        for ( const auto &formatter : filename_formatters_ )
            formatter->format( msg, file_name );
        file_ = std::ofstream( file_name, truncate_ ? std::ios_base::trunc : std::ios_base::app );
        files_.push_file( file_name );
    }

    void SizeFileSink::log( const Message &message )
    {
        const auto msg = write( message );
        std::size_t size = max_file_size_;
        if ( std::filesystem::exists( files_.get_current_file( ) ) )
            size = std::filesystem::file_size( files_.get_current_file( ) );

        if ( size + msg.size( ) > max_file_size_ )
        {
            // new file
            std::string file_name;
            for ( const auto &formatter : filename_formatters_ )
                formatter->format( message, file_name );
            file_ = std::ofstream( file_name, truncate_ ? std::ios_base::trunc : std::ios_base::app );
            files_.push_file( file_name );
        }
        file_ << msg << std::endl;
    }

    void SizeFileSink::flush( )
    {
        file_ << std::flush;
    }
} // namespace vrock::log