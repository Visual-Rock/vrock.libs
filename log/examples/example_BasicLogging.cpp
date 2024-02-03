#include <vrock/log.hpp>
using namespace vrock::log;

int main( )
{
    const auto logger = make_logger( "EXAMPLE", LogLevel::Trace );
    logger->add_sink( std::make_shared<StandardOutSink>( ) );

    logger->trace( "trace" );
    logger->debug( "debug" );
    logger->info( "info" );
    logger->warn( "warn" );
    logger->error( "error" );
    logger->critical( "critical" );

    return 0;
}