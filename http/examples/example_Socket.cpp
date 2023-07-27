#include <chrono>
#include <iostream>
#include <thread>

import vrock.http.HttpServer;
import vrock.utils.Timer;
import vrock.utils.ByteArray;

using namespace vrock::http;

int main( )
{
    vrock::utils::ScopedTimer timer( []( auto time ) { std::cout << "took " << time << "ms!\n"; } );

    HttpServer server( "0.0.0.0", 8080 );

    server.run( );

    std::string command;
    while ( std::cin >> command, command != "quit" )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    server.stop( );
    return 0;
}