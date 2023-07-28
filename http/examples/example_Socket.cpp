#include <chrono>
#include <iostream>
#include <thread>

import vrock.http.HttpServer;
import vrock.http.HttpMessage;
import vrock.utils.Timer;
import vrock.utils.ByteArray;

using namespace vrock::http;

auto get_hello( const HttpMessage &msg ) -> HttpResponse
{
    std::cout << "Hello" << std::endl;
    HttpResponse response;
    response.status_code = HttpStatusCode::Ok;
    response.body = "Hello World!";
    return response;
}

int main( )
{
    vrock::utils::ScopedTimer timer( []( auto time ) { std::cout << "took " << time << "ms!\n"; } );

    HttpServer server( "0.0.0.0", 8080 );

    server.add_endpoint( "/hello", HttpMethod::Get, get_hello );

    server.run( );

    std::string command;
    while ( std::cin >> command, command != "quit" )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    server.stop( );
    return 0;
}