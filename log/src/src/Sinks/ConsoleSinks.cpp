#include "vrock/log/Sinks/ConsoleSinks.hpp"

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
        std::cout << write( message ) << std::endl;
    }
} // namespace vrock::log
