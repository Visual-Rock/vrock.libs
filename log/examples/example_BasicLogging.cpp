#define VROCKLIBS_LOG_USE_THREAD_ID
#define VROCKLIBS_LOG_USE_PROCESS_ID
#include <vrock/log.hpp>
#include <vrock/utils.hpp>
using namespace vrock::log;
using namespace std::chrono;

constexpr int iterations = 100000;

int main( )
{
    auto logger = Logger( LogLevel::Trace, "EXAMPLE" );
    logger.set_pattern( "[ %x %T ] %v" );
    logger.add_sink<StandardOutSink>( );

    //    {
    //        vrock::utils::ScopedTimer<nanoseconds> timer(
    //            [ & ]( auto x ) { logger.info( "logging took {}", x / iterations ); } );
    //
    //        for ( int i = 0; i < iterations; ++i )
    //        {
    //            logger.warn( "Test" );
    //        }
    //    }

    logger.trace( "trace" );
    logger.debug( "debug" );
    logger.info( "info" );
    logger.warn( "warn" );
    logger.error( "error" );
    logger.critical( "critical" );

    return 0;
}