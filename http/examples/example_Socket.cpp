#include <chrono>
#include <iostream>
#include <thread>

import vrock.http.HttpServer;
import vrock.http.Interceptor;
import vrock.http.HttpMessage;
import vrock.utils.Timer;
import vrock.utils.ByteArray;

using namespace vrock::http;

struct RequestData
{
    std::size_t i = 0;
};

class LogReqest : public RequestInterceptor<RequestData>
{
public:
    LogReqest( ) = default;

    auto incoming( HttpRequest<RequestData> &request ) -> void final
    {
        static std::size_t req_count = 0;
        request.data.i = req_count++;
        std::cout << std::format( "{}: {} {}", request.data.i, to_string( request.method ), request.path ) << std::endl;
    }
};

auto get_hello( const HttpMessage &msg ) -> HttpResponse<RequestData>
{
    HttpResponse<RequestData> response;
    response.status_code = HttpStatusCode::Ok;
    response.body = "Hello World!";
    return response;
}

int main( )
{
    vrock::utils::ScopedTimer timer( []( auto time ) { std::cout << "took " << time << "ms!\n"; } );

    HttpServer<RequestData> server( "0.0.0.0", 8080 );

    server.add_endpoint( "/hello", HttpMethod::Get, get_hello );
    server.add_request_interceptor( std::make_shared<LogReqest>( ) );
    auto cors = std::make_shared<CorsIntercepter<RequestData>>( );
    cors->allowed_headers = { "Content", "Test" };
    cors->allow_credentials = true;
    cors->expose_headers = { "*" };
    server.add_response_interceptor( cors );
    server.set_file_extensions( { ".html", ".js", ".css" } );
    server.serve_files( false );
    server.run( );

    std::string command;
    while ( std::cin >> command, command != "quit" )
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

    server.stop( );
    return 0;
}