module;

#include <format>
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
import vrock.http.Interceptor;

using Socket = vrock::networking::Socket;

namespace vrock::http
{
    export class HttpError : std::exception
    {
    public:
        HttpError( HttpStatusCode c, std::string b ) : code( c ), body( std::move( b ) )
        {
            msg = std::format( "{}: {}", to_string( code ), body );
        }

        [[nodiscard]] const char *what( ) const noexcept override
        {
            return msg.c_str( );
        }

        HttpStatusCode code;
        std::string body;

    private:
        std::string msg;
    };

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
            if ( !path_cache.contains( path ) )
            {
                endpoints.push_back( { compile_path( path ), { { method, fn } } } );
                path_cache[ path ] = endpoints.size( ) - 1;
            }
            else
                endpoints[ path_cache[ path ] ].second[ method ] = std::move( fn );
        }

        auto add_request_interceptor( std::shared_ptr<RequestInterceptor> interceptor ) -> void
        {
            req_interceptors.push_back( interceptor );
        }

        auto add_response_interceptor( std::shared_ptr<ResponseInterceptor> interceptor ) -> void
        {
            res_interceptors.push_back( interceptor );
        }

        auto add_interceptor( std::shared_ptr<Interceptor> interceptor ) -> void
        {
            req_interceptors.push_back( interceptor );
            res_interceptors.push_back( interceptor );
        }

        auto handle_exception( HttpRequest &req, std::exception &exception ) -> HttpResponse
        {
            HttpResponse res;
            res.status_code = HttpStatusCode::InternalServerError;
            res.version = HttpVersion::HTTP_1_1;
            res.body = exception.what( );
            return res;
        }

    protected:
        auto handle_request( const Socket &socket, const vrock::utils::ByteArray<> &data ) -> bool override
        {
            HttpRequest req;
            try
            {
                HttpParser parser;
                auto str = data.to_string( );
                req = parser.parse_request( str );
                auto path_segments = split_path( req.path );
                bool match = false;
                for ( auto &[ matcher, map ] : endpoints )
                {
                    if ( path_segments.size( ) != matcher.size( ) )
                        continue;
                    for ( int i = 0; i < path_segments.size( ); ++i )
                        if ( !( match = matcher[ i ]->match( path_segments[ i ] ) ) )
                            break;
                    if ( !match )
                        continue;
                    for ( int i = 0; i < path_segments.size( ); ++i )
                        matcher[ i ]->get_parameters( req.parameters, path_segments[ i ] );

                    for ( auto &i : req_interceptors )
                        i->incoming( req );
                    auto res = map[ req.method ]( req );
                    for ( auto &i : res_interceptors )
                        i->outgoing( res );

                    socket.send( utils::ByteArray( to_string( res ) ) );
                    return true;
                }
                return true;
            }
            catch ( HttpError &err )
            {
                HttpResponse res;
                res.status_code = err.code;
                res.version = HttpVersion::HTTP_1_1;
                res.body = err.body;
                socket.send( utils::ByteArray( to_string( res ) ) );
                return true;
            }
            catch ( std::exception &e )
            {
                HttpResponse res = exception_handler( req, e );
                socket.send( utils::ByteArray( to_string( res ) ) );
                return true;
            }
        }

        std::vector<std::pair<std::vector<std::shared_ptr<BaseMatcher>>,
                              std::unordered_map<HttpMethod, std::function<HttpResponse( const HttpRequest & )>>>>
            endpoints;

        std::unordered_map<std::string, std::size_t> path_cache;

        std::vector<std::shared_ptr<RequestInterceptor>> req_interceptors = { };
        std::vector<std::shared_ptr<ResponseInterceptor>> res_interceptors = { };
        std::function<HttpResponse( HttpRequest &, std::exception & )> exception_handler =
            std::bind( &HttpServer::handle_exception, this, std::placeholders::_1, std::placeholders::_2 );
    };
} // namespace vrock::http