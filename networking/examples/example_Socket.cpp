#include <chrono>
#include <iostream>
#include <thread>

import vrock.networking.Server;
import vrock.networking.Sockets;
using namespace vrock::networking;

int main( )
{
    BaseServer server = BaseServer( Domain::IPv4, Type::TCP, 8080, 10 );

    server.run( );

    std::string command;
    while ( std::cin >> command, command != "quit" )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    server.stop( );

    return 0;
}