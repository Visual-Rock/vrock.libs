#include <iostream>

import vrock.log.Logger;
using namespace vrock::log;

int main( )
{
    LoggerConfig cfg = LoggerConfig( "Example" ).set_log_level( LogLevel::Error ).add_console_colored( );
    auto &logger = create_logger( cfg );
    auto &logger2 = get_logger( "Example" );

    logger.info( "should not be logged" );
    logger2.error( "should be logged" );
    return 0;
}