module;

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

export module vrock.server.HttpMessage;

namespace vrock::server
{
    export enum class HttpMethod
    {
        Get,
        Head,
        Post,
        Put,
        Delete,
        Connect,
        Options,
        Trace,
        Patch
    };

    export enum class HttpVersion
    {
        HTTP_0_9 = 9,
        HTTP_1_0 = 10,
        HTTP_1_1 = 11,
        HTTP_2_0 = 20,
    };

    export enum class HttpStatusCode
    {
        Continue = 100,
        SwitchingProtocols = 101,
        EarlyHints = 103,
        Ok = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        NotModified = 304,
        BadRequest = 400,
        Unauthorized = 401,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        RequestTimeout = 408,
        ImATeapot = 418,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnvailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505
    };

    export constexpr auto to_string( HttpMethod method ) noexcept -> std::string
    {
        switch ( method )
        {
        case HttpMethod::Get:
            return "GET";
        case HttpMethod::Head:
            return "HEAD";
        case HttpMethod::Post:
            return "POST";
        case HttpMethod::Put:
            return "PUT";
        case HttpMethod::Delete:
            return "DELETE";
        case HttpMethod::Connect:
            return "CONNECT";
        case HttpMethod::Options:
            return "OPTIONS";
        case HttpMethod::Trace:
            return "TRACE";
        case HttpMethod::Patch:
            return "PATCH";
        default:
            return { };
        }
    }

    export constexpr auto to_string( HttpVersion version ) noexcept -> std::string
    {
        switch ( version )
        {
        case HttpVersion::HTTP_0_9:
            return "HTTP/0.9";
        case HttpVersion::HTTP_1_0:
            return "HTTP/1.0";
        case HttpVersion::HTTP_1_1:
            return "HTTP/1.1";
        case HttpVersion::HTTP_2_0:
            return "HTTP/2.0";
        default:
            return { };
        }
    }

    export auto to_method( const std::string &method_string ) -> HttpMethod
    {
        std::string method_string_uppercase;
        std::transform( method_string.begin( ), method_string.end( ), std::back_inserter( method_string_uppercase ),
                        []( char c ) { return toupper( c ); } );

        if ( method_string_uppercase == "GET" )
            return HttpMethod::Get;
        else if ( method_string_uppercase == "HEAD" )
            return HttpMethod::Head;
        else if ( method_string_uppercase == "POST" )
            return HttpMethod::Post;
        else if ( method_string_uppercase == "PUT" )
            return HttpMethod::Put;
        else if ( method_string_uppercase == "DELETE" )
            return HttpMethod::Delete;
        else if ( method_string_uppercase == "CONNECT" )
            return HttpMethod::Connect;
        else if ( method_string_uppercase == "OPTIONS" )
            return HttpMethod::Options;
        else if ( method_string_uppercase == "TRACE" )
            return HttpMethod::Trace;
        else if ( method_string_uppercase == "PATCH" )
            return HttpMethod::Patch;
        else
            throw std::invalid_argument( "Unexpected HTTP method" );
    }

    export auto string_to_version( const std::string &version_string ) -> HttpVersion
    {
        std::string version_string_uppercase;
        std::transform( version_string.begin( ), version_string.end( ), std::back_inserter( version_string_uppercase ),
                        []( char c ) { return toupper( c ); } );

        if ( version_string_uppercase == "HTTP/0.9" )
            return HttpVersion::HTTP_0_9;
        else if ( version_string_uppercase == "HTTP/1.0" )
            return HttpVersion::HTTP_1_0;
        else if ( version_string_uppercase == "HTTP/1.1" )
            return HttpVersion::HTTP_1_1;
        else if ( version_string_uppercase == "HTTP/2" || version_string_uppercase == "HTTP/2.0" )
            return HttpVersion::HTTP_2_0;
        else
            throw std::invalid_argument( "Unexpected HTTP version" );
    }

    export class HttpMessage
    {
    public:
        HttpVersion version = HttpVersion::HTTP_1_1;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    export class HttpRequest : public HttpMessage
    {
    public:
        HttpMethod method;
        std::string uri;
    };

    export class HttpResponse : public HttpMessage
    {
    public:
        HttpStatusCode status_code;
    };
} // namespace vrock::server
