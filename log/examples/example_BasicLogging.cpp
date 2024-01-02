#include <iostream>

#define VROCKLIBS_LOG_USE_THREAD_ID
#define VROCKLIBS_LOG_USE_PROCESS_ID
#include <vrock/log.hpp>
#include <vrock/utils.hpp>
using namespace vrock::utils;

int main( )
{
    auto logger = vrock::log::Logger( "EXAMPLE", vrock::log::LogLevel::Trace );
    logger.add_sink<vrock::log::ConsoleSink>( "[ %!:%# ] %*[ %n ]%$ %^[ %l ]%$ %v" );

    logger.trace( "trace" );
    logger.debug( "debug" );
    logger.info( "info" );
    logger.warn( "warn" );
    logger.error( "error" );
    logger.critical( "critical" );

    return 0;
}