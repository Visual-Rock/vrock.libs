module;

#include <algorithm>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

export module vrock.http.HttpMessage;

import vrock.http.Path;

namespace vrock::http
{
    static std::unordered_map<std::uint16_t, std::string> status_code_map = {
        { 100, "Continue" },
        { 101, "Switching Protocols" },
        { 102, "Processing" },
        { 200, "OK" },
        { 201, "Created" },
        { 202, "Accepted" },
        { 203, "Non-Authoritative Information" },
        { 204, "No Content" },
        { 205, "Reset Content" },
        { 206, "Partial Content" },
        { 207, "Multi-Status" },
        { 208, "Already Reported" },
        { 226, "IM Used" },
        { 300, "Multiple Choices" },
        { 301, "Moved Permanently" },
        { 302, "Found" },
        { 303, "See Other" },
        { 304, "Not Modified" },
        { 305, "Use Proxy" },
        { 306, "Reserved" },
        { 307, "Temporary Redirect" },
        { 308, "Permanent Redirect" },
        { 400, "Bad Request" },
        { 401, "Unauthorized" },
        { 402, "Payment Required" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
        { 405, "Method Not Allowed" },
        { 406, "Not Acceptable" },
        { 407, "Proxy Authentication Required" },
        { 408, "Request Timeout" },
        { 409, "Conflict" },
        { 410, "Gone" },
        { 411, "Length Required" },
        { 412, "Precondition Failed" },
        { 413, "Request Entity Too Large" },
        { 414, "Request-URI Too Long" },
        { 415, "Unsupported Media Type" },
        { 416, "Requested Range Not Satisfiable" },
        { 417, "Expectation Failed" },
        { 422, "Unprocessable Entity" },
        { 423, "Locked" },
        { 424, "Failed Dependency" },
        { 426, "Upgrade Required" },
        { 428, "Precondition Required" },
        { 429, "Too Many Requests" },
        { 431, "Request Header Fields Too Large" },
        { 500, "Internal Server Error" },
        { 501, "Not Implemented" },
        { 502, "Bad Gateway" },
        { 503, "Service Unavailable" },
        { 504, "Gateway Timeout" },
        { 505, "HTTP Version Not Supported" },
        { 506, "Variant Also Negotiates (Experimental)" },
        { 507, "Insufficient Storage" },
        { 508, "Loop Detected" },
        { 510, "Not Extended" },
        { 511, "Network Authentication Required" },
    };

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

    export constexpr auto to_string( HttpStatusCode code ) noexcept -> std::string
    {
        return status_code_map[ (std::uint16_t)code ];
    }

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

    export constexpr auto to_method( const std::string &method_string ) -> HttpMethod
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

    export constexpr auto to_version( const std::string &version_string ) -> HttpVersion
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

    export struct EmptyRequestData
    {
    };

    export class HttpMessage
    {
    public:
        HttpVersion version = HttpVersion::HTTP_1_1;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
        bool keep_alive = false;
    };

    export template <class T>
        requires std::is_default_constructible_v<T>
    class HttpRequest : public HttpMessage
    {
    public:
        HttpMethod method;
        std::string path;
        Parameters parameters;

        T data;
    };

    export template <class T>
        requires std::is_default_constructible_v<T>
    class HttpResponse : public HttpMessage
    {
    public:
        HttpResponse( ) : status_code( HttpStatusCode::Continue ), request( { } )
        {
        }

        HttpResponse( HttpStatusCode code, HttpRequest<T> req ) : status_code( code ), request( std::move( req ) )
        {
        }

        HttpStatusCode status_code;
        HttpRequest<T> request;
    };

    export template <class T>
        requires std::is_default_constructible_v<T>
    auto to_string( HttpResponse<T> &res ) -> std::string
    {
        std::string str;
        res.headers[ "Content-Length" ] = std::to_string( res.body.size( ) );
        str += std::format( "{} {} {}\r\n", to_string( res.version ), std::to_string( (std::uint16_t)res.status_code ),
                            to_string( res.status_code ) );
        for ( const auto &[ k, v ] : res.headers )
            str += std::format( "{}: {}\r\n", k, v );
        str += "\r\n";
        str += res.body;
        // str += "\r\n";
        return str;
    }
} // namespace vrock::http