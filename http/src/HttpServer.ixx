module;

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

export module vrock.http.HttpServer;

import vrock.http.TCPServer;
import vrock.networking.Sockets;
import vrock.utils.ByteArray;
import vrock.http.Path;
import vrock.http.HttpMessage;
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

        auto add_endpoint( const std::string &path, HttpMethod method,
                           std::function<HttpResponse( const HttpRequest & )> fn ) -> void
        {
            auto res = path_cache.find( path );
            if ( res == path_cache.end( ) )
            {
                endpoints.push_back( { compile_path( path ), { { method, fn } } } );
                path_cache[ path ] = endpoints.size( ) - 1;
            }
            else
                endpoints[ res->second ].second[ method ] = std::move( fn );
        }

    protected:
        auto handle_request( const Socket &socket, const vrock::utils::ByteArray<> &data ) -> bool override
        {
            HttpParser parser;
            auto str = data.to_string( );
            auto req = parser.parse_request( str );
            auto path_segments = split_path( req.path );
            bool match = false;
            for ( auto &e : endpoints )
            {
                auto &[ matcher, map ] = e;
                if ( path_segments.size( ) != matcher.size( ) )
                    continue;
                for ( int i = 0; i < path_segments.size( ); ++i )
                    if ( !( match = matcher[ i ]->match( path_segments[ i ] ) ) )
                        break;
                if ( !match )
                    continue;
                for ( int i = 0; i < path_segments.size( ); ++i )
                    matcher[ i ]->get_parameters( req.parameters, path_segments[ i ] );
                socket.send( arr );
                auto res = map[ req.method ]( req );
                return true;
            }
            socket.send( arr );
            return true;
        }

        std::vector<std::pair<std::vector<std::shared_ptr<BaseMatcher>>,
                              std::unordered_map<HttpMethod, std::function<HttpResponse( const HttpRequest & )>>>>
            endpoints;

        std::unordered_map<std::string, std::size_t> path_cache;
    };
} // namespace vrock::http