#include <iostream>
#include <thread>

import vrock.server.HttpServer;
#include <format>
using HttpServer = vrock::server::HttpServer;

int main( )
{
    HttpServer server( "0.0.0.0", 8080, 8 );

    server.add_route( "/hello", vrock::server::HttpMethod::Get, []( const auto &req ) {
        auto res = vrock::server::HttpResponse( );
        res.body = "hello";
        res.status_code = vrock::server::HttpStatusCode::Ok;
        res.headers[ "Content-Type" ] = "text/plain";
        return vrock::server::HttpResponse( );
    } );

    server.run( );

    std::cout << std::format( "running server on port {} with {} threads", server.port( ), server.threads( ) )
              << std::endl;

    std::string command;
    while ( std::cin >> command, command != "quit" )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    server.stop( );

    return 0;
}