#include <vrock/log.hpp>
using namespace vrock::log;

int main( )
{
    auto logger = Logger( LogLevel::Trace, "EXAMPLE" );
    logger.set_pattern( "[ %x %T ] %v" );
    logger.add_sink<StandardOutSink>( );

    logger.trace( "trace" );
    logger.debug( "debug" );
    logger.info( "info" );
    logger.warn( "warn" );
    logger.error( "error" );
    logger.critical( "critical" );

    return 0;
}