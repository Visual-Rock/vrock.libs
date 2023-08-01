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

#include <algorithm>
#include <filesystem>
#include <fstream>

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

    export template <class RequestData = EmptyRequestData, std::size_t MaxEvents = 10000,
                     std::size_t BacklogSize = 1000>
    class HttpServer : public TCPServer<MaxEvents, BacklogSize>
    {
    public:
        HttpServer( std::string host, std::uint16_t port,
                    std::size_t max_threads = std::max( 1u, std::thread::hardware_concurrency( ) / 2 ) )
            : TCPServer<MaxEvents, BacklogSize>( host, port )
        {
        }

        auto add_endpoint( const std::string &path, HttpMethod method,
                           std::function<HttpResponse<RequestData>( const HttpRequest<RequestData> & )> fn ) -> void
        {
            if ( !path_cache.contains( path ) )
            {
                endpoints.push_back( { compile_path( path ), { { method, fn } } } );
                path_cache[ path ] = endpoints.size( ) - 1;
            }
            else
                endpoints[ path_cache[ path ] ].second[ method ] = std::move( fn );
        }

        auto serve_files( bool preload ) -> void
        {
            files = true;
            if ( preload )
            {
                const std::filesystem::path curr{ "./" };
                for ( auto const &dir_entry : std::filesystem::recursive_directory_iterator{ curr } )
                {
                    if ( dir_entry.is_regular_file( ) )
                    {
                        if ( file_extensions.empty( ) )
                            load_file( dir_entry.path( ) );
                        else
                            for ( const auto &ext : file_extensions )
                                if ( dir_entry.path( ).extension( ).string( ) == ext )
                                    load_file( dir_entry.path( ) );
                    }
                }
            }
        }

        auto set_file_extensions( const std::vector<std::string> &ext ) -> void
        {
            file_extensions = ext;
        }

        auto add_request_interceptor( std::shared_ptr<RequestInterceptor<RequestData>> interceptor ) -> void
        {
            req_interceptors.push_back( interceptor );
        }

        auto add_response_interceptor( std::shared_ptr<ResponseInterceptor<RequestData>> interceptor ) -> void
        {
            res_interceptors.push_back( interceptor );
        }

        auto add_interceptor( std::shared_ptr<Interceptor<RequestData>> interceptor ) -> void
        {
            req_interceptors.push_back( interceptor );
            res_interceptors.push_back( interceptor );
        }

        auto handle_exception( HttpRequest<RequestData> &req, std::exception &exception ) -> HttpResponse<RequestData>
        {
            HttpResponse<RequestData> res;
            res.status_code = HttpStatusCode::InternalServerError;
            res.version = HttpVersion::HTTP_1_1;
            res.body = exception.what( );
            return res;
        }

    protected:
        auto handle_request( const Socket &socket, const vrock::utils::ByteArray<> &data ) -> bool override
        {
            HttpRequest<RequestData> req;
            try
            {
                HttpParser parser;
                auto str = data.to_string( );
                // parse request
                auto r = parser.parse_request( str );
                req.path = std::move( r.path );
                req.method = r.method;
                req.parameters = std::move( r.parameters );
                req.body = std::move( r.body );
                req.version = r.version;
                req.headers = std::move( r.headers );
                req.data = RequestData( );

                // parse path
                auto path_segments = split_path( req.path );

                // search for endpoint handler
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
                    auto e = map.find( req.method );
                    if ( e == map.end( ) )
                        throw HttpError( HttpStatusCode::MethodNotAllowed, "" );
                    auto res = map[ req.method ]( req );
                    for ( auto &i : res_interceptors )
                        i->outgoing( res );

                    socket.send( utils::ByteArray( to_string( res ) ) );
                    return true;
                }
                if ( files && req.method == HttpMethod::Get )
                {
                    auto res = get_file_response( req );
                    socket.send( utils::ByteArray( to_string( res ) ) );
                    return true;
                }
                throw HttpError( HttpStatusCode::NotFound, "" );
            }
            catch ( HttpError &err )
            {
                HttpResponse<RequestData> res;
                res.status_code = err.code;
                res.version = HttpVersion::HTTP_1_1;
                res.body = err.body;
                socket.send( utils::ByteArray( to_string( res ) ) );
                return true;
            }
            catch ( std::exception &e )
            {
                HttpResponse<RequestData> res = exception_handler( req, e );
                socket.send( utils::ByteArray( to_string( res ) ) );
                return true;
            }
        }

        auto load_file( const std::filesystem::path &path ) -> void
        {
            std::ifstream t( path.string( ) );
            t.seekg( 0, std::ios::end );
            size_t size = t.tellg( );
            std::string buffer( size, ' ' );
            t.seekg( 0 );
            t.read( &buffer[ 0 ], size );

            file_cache[ path.string( ).substr( 1 ) ] = buffer;
            std::cout << std::format( "{} {} {}", path.string( ), path.extension( ).string( ),
                                      path.filename( ).string( ) )
                      << std::endl;
        }

        auto get_file_response( HttpRequest<RequestData> &req ) -> HttpResponse<RequestData>
        {
            auto f = file_cache.find( req.path );
            if ( f == file_cache.end( ) )
            {
                std::filesystem::path p{ "." + req.path };
                if ( !( is_regular_file( p ) &&
                        ( file_extensions.empty( ) || std::find( file_extensions.begin( ), file_extensions.end( ),
                                                                 p.extension( ) ) != file_extensions.end( ) ) ) )
                    throw HttpError( HttpStatusCode::NotFound, "" );
                load_file( p );
            }
            for ( auto &i : req_interceptors )
                i->incoming( req );
            auto res = HttpResponse<RequestData>( );
            res.status_code = HttpStatusCode::Ok;
            res.body = file_cache[ req.path ];
            for ( auto &i : res_interceptors )
                i->outgoing( res );
            return res;
        }

        std::vector<std::pair<std::vector<std::shared_ptr<BaseMatcher>>,
                              std::unordered_map<HttpMethod, std::function<HttpResponse<RequestData>(
                                                                 const HttpRequest<RequestData> & )>>>>
            endpoints;

        std::unordered_map<std::string, std::size_t> path_cache;

        std::vector<std::shared_ptr<RequestInterceptor<RequestData>>> req_interceptors = { };
        std::vector<std::shared_ptr<ResponseInterceptor<RequestData>>> res_interceptors = { };
        std::function<HttpResponse<RequestData>( HttpRequest<RequestData> &, std::exception & )> exception_handler =
            std::bind( &HttpServer::handle_exception, this, std::placeholders::_1, std::placeholders::_2 );

        bool files = false;
        std::vector<std::string> file_extensions = { };
        std::unordered_map<std::string, std::string> file_cache = { };
    };
} // namespace vrock::http