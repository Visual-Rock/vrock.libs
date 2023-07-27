module;

#include <iostream>
#include <string>
#include <thread>

export module vrock.http.HttpServer;

import vrock.http.TCPServer;
import vrock.networking.Sockets;
import vrock.utils.ByteArray;
import vrock.http.HttpParser;

using Socket = vrock::networking::Socket;

namespace vrock::http
{
    vrock::utils::ByteArray arr = vrock::utils::ByteArray( "HTTP/1.1 200 OK\r\n"
                                                           "Server: Hello\r\n"
                                                           "Content-Length: 13\r\n"
                                                           "Content-Type: text/plain\r\n"
                                                           "\r\n"
                                                           "Hello, world\r\n" );

    export template <std::size_t MaxEvents = 10000, std::size_t BacklogSize = 1000>
    class HttpServer : public TCPServer<MaxEvents, BacklogSize>
    {
    public:
        HttpServer( std::string host, std::uint16_t port,
                    std::size_t max_threads = std::max( 1u, std::thread::hardware_concurrency( ) / 2 ) )
            : TCPServer<MaxEvents, BacklogSize>( host, port )
        {
        }

    protected:
        auto handle_request( const Socket &socket, const vrock::utils::ByteArray<> &data ) -> bool override
        {
            HttpParser parser;
            auto str = data.to_string( );
            auto req = parser.parse_request( str );
            auto res = parser.parse_response( arr.to_string( ) );
            socket.send( arr );
            return true;
        }
    };
} // namespace vrock::http