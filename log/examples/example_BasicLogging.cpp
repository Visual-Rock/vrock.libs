#include <vrock/log.hpp>
using namespace vrock::log;

int main( )
{
    auto logger = Logger( "EXAMPLE", LogLevel::Trace );
    logger.add_sink( std::make_shared<StandardOutSink>( ) );
    logger.add_sink( std::make_shared<SizeFileSink>( "%d-%m-%YT%T.txt", 4 * 1024 * 1024, 1 ) );

    logger.trace( "trace" );
    logger.debug( "debug" );
    logger.info( "info" );
    logger.warn( "warn" );
    logger.error( "error" );
    logger.critical( "critical" );

    while ( true )
    {
        logger.trace( "trace" );
        //        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    }

    return 0;
}