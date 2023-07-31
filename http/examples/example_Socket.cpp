#include <chrono>
#include <iostream>
#include <thread>

import vrock.http.HttpServer;
import vrock.http.Interceptor;
import vrock.http.HttpMessage;
import vrock.utils.Timer;
import vrock.utils.ByteArray;

using namespace vrock::http;

class LogReqest : public RequestInterceptor
{
public:
    LogReqest( ) = default;

    auto incoming( HttpRequest &request ) -> void final
    {
        std::cout << std::format( "{} {}", to_string( request.method ), request.path ) << std::endl;
    }
};

auto get_hello( const HttpMessage &msg ) -> HttpResponse
{
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
    server.add_request_interceptor( std::make_shared<LogReqest>( ) );
    auto cors = std::make_shared<CorsIntercepter>( );
    cors->allowed_headers = { "Content", "Test" };
    cors->allow_credentials = true;
    cors->expose_headers = { "*" };
    server.add_response_interceptor( cors );

    server.run( );

    std::string command;
    while ( std::cin >> command, command != "quit" )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    server.stop( );
    return 0;
}