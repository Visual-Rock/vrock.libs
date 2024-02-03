#include <vrock/log.hpp>
using namespace vrock::log;

int main( )
{
    auto logger = make_logger( "example" );
    logger->add_sink( std::make_shared<StandardOutSink>( ) );
    logger->info( "Hello, World!" );

    return 0;
}