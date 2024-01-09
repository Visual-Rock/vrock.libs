#include "vrock/log/Sinks/ConsoleSinks.hpp"

#include <iostream>

#ifdef WIN32
#include "Windows.h"
#endif

namespace vrock::log
{
    auto enable_color_output( ) -> void
    {
#ifdef WIN32
        HANDLE hOutput = GetStdHandle( STD_OUTPUT_HANDLE );
        if ( hOutput == INVALID_HANDLE_VALUE )
            throw std::runtime_exception( "failed to get output handle." );

        SetConsoleMode( hOutput, ENABLE_VIRTUAL_TERMINAL_PROCESSING );
#endif
    }

    StandardOutSink::StandardOutSink( std::string_view pattern, bool use_ansi ) : Sink( pattern, use_ansi )
    {
        if ( use_ansi )
            enable_color_output( );
    }

    auto StandardOutSink::log( const Message &message ) -> void
    {
        // TODO: Use std::print when compiler support is available
        std::cout << write( message ) << std::endl;
    }

    auto StandardOutSink::flush( ) -> void
    {
        std::cout << std::flush;
    }

    StandardErrSink::StandardErrSink( std::string_view pattern, LogLevel level, bool use_ansi )
        : Sink( pattern, use_ansi ), level_( level )
    {
    }

    auto StandardErrSink::log( const Message &message ) -> void
    {
        if ( includes_level( level_, message.level ) )
        {
            std::cerr << write( message ) << std::endl;
        }
    }

    auto StandardErrSink::flush( ) -> void
    {
        std::cerr << std::flush;
    }
} // namespace vrock::log
