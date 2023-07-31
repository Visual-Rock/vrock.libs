module;

#include <cstdint>
#include <numeric>
#include <string>
#include <vector>

export module vrock.http.Interceptor;

import vrock.http.HttpMessage;

namespace vrock::http
{
    export class RequestInterceptor
    {
    public:
        virtual auto incoming( HttpRequest &request ) -> void = 0;
    };

    export class ResponseInterceptor
    {
    public:
        virtual auto outgoing( HttpResponse &response ) -> void = 0;
    };

    export class Interceptor : public RequestInterceptor, public ResponseInterceptor
    {
    };

    export class CorsIntercepter : public ResponseInterceptor
    {
    public:
        CorsIntercepter( ) = default;

        auto outgoing( HttpResponse &response ) -> void final
        {
            response.headers[ "Access-Control-Allow-Origin" ] = allowed_origin;

            if ( allow_credentials )
                response.headers[ "Access-Control-Allow-Credentials" ] = "true";

            if ( !allowed_headers.empty( ) )
                response.headers[ "Access-Control-Allow-Headers" ] =
                    std::accumulate( allowed_headers.begin( ) + 1, allowed_headers.end( ), allowed_headers[ 0 ],
                                     []( const auto &a, auto &b ) { return a + ", " + b; } );

            if ( !allowed_methods.empty( ) )
                response.headers[ "Access-Control-Allow-Methods" ] = std::accumulate(
                    allowed_methods.begin( ) + 1, allowed_methods.end( ), to_string( allowed_methods[ 0 ] ),
                    []( const auto &a, auto &b ) { return a + ", " + to_string( b ); } );
            else
                response.headers[ "Access-Control-Allow-Methods" ] = "*";

            response.headers[ "Access-Control-Max-Age" ] = std::to_string( max_age );

            if ( !expose_headers.empty( ) )
                response.headers[ "Access-Control-Expose-Headers" ] =
                    std::accumulate( expose_headers.begin( ) + 1, expose_headers.end( ), expose_headers[ 0 ],
                                     []( const auto &a, auto &b ) { return a + ", " + b; } );
        }

        std::string allowed_origin = "*";
        bool allow_credentials = false;
        std::uint64_t max_age = 86400;
        std::vector<HttpMethod> allowed_methods = { };
        std::vector<std::string> allowed_headers = { };
        std::vector<std::string> expose_headers = { };
    };
} // namespace vrock::http