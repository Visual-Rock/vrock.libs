module;

#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

#include "llhttp/llhttp.h"

export module vrock.http.HttpParser;

import vrock.utils.ByteArray;
import vrock.http.HttpMessage;
import vrock.http.Url;

namespace vrock::http
{
    struct ParseData
    {
        std::string last_header;
        bool is_request = false;
        void *http_msg;
    };

    auto on_url( llhttp_t *p, const char *at, size_t length ) -> int
    {
        if ( p->data )
        {
            auto *data = (ParseData *)p->data;
            if ( !data->is_request )
                return HPE_USER;
            auto *req = (HttpRequest *)data->http_msg;
            req->url = Url( std::string( at, length ) );
            return 0;
        }
        return HPE_USER;
    }

    auto on_method( llhttp_t *p, const char *at, size_t length ) -> int
    {
        if ( p->data )
        {
            auto *data = (ParseData *)p->data;
            if ( !data->is_request )
                return HPE_USER;
            auto *req = (HttpRequest *)data->http_msg;
            req->method = to_method( std::string( at, length ) );
            return 0;
        }
        return HPE_USER;
    }

    auto on_version( llhttp_t *p, const char *at, size_t length ) -> int
    {
        if ( p->data )
        {
            auto *data = (ParseData *)p->data;
            auto *msg = (HttpMessage *)data->http_msg;
            msg->version = to_version( "http/" + std::string( at, length ) );
            return 0;
        }
        return HPE_USER;
    }

    auto on_header_field( llhttp_t *p, const char *at, size_t length ) -> int
    {
        if ( p->data )
        {
            auto *data = (ParseData *)p->data;
            data->last_header = std::string( at, length );
            return 0;
        }
        return HPE_USER;
    }

    auto on_header_value( llhttp_t *p, const char *at, size_t length ) -> int
    {
        if ( p->data )
        {
            auto *data = (ParseData *)p->data;
            auto *msg = (HttpMessage *)data->http_msg;
            msg->headers[ data->last_header ] = std::string( at, length );
            return 0;
        }
        return HPE_USER;
    }

    auto on_body( llhttp_t *p, const char *at, size_t length ) -> int
    {
        if ( p->data )
        {
            auto *data = (ParseData *)p->data;
            auto *msg = (HttpMessage *)data->http_msg;
            msg->body = std::string( at, length );
            return 0;
        }
        return HPE_USER;
    }

    export class HttpParser
    {
    public:
        HttpParser( )
        {
            llhttp_settings_init( &settings );
            settings.on_method = on_method;
            settings.on_url = on_url;
            settings.on_header_field = on_header_field;
            settings.on_header_value = on_header_value;
            settings.on_body = on_body;
            settings.on_version = on_version;
        }

        auto parse_request( const std::string &req ) -> HttpRequest
        {
            llhttp_t parser;
            llhttp_init( &parser, HTTP_REQUEST, &settings );

            auto request = HttpRequest( );
            ParseData d{ };
            d.http_msg = &request;
            d.is_request = true;
            parser.data = &d;
            llhttp_reset( &parser );
            enum llhttp_errno err = llhttp_execute( &parser, req.c_str( ), req.size( ) );
            if ( err == HPE_OK )
                return request;
            else
                throw std::runtime_error(
                    std::format( "failed to parse HTTP request: {} {}", llhttp_errno_name( err ), parser.reason ) );
        }
        auto parse_response( const std::string &res ) -> HttpResponse
        {
            llhttp_t parser;
            llhttp_init( &parser, HTTP_RESPONSE, &settings );

            auto response = HttpResponse( );
            ParseData d{ };
            d.http_msg = &response;
            d.is_request = true;
            parser.data = &d;
            llhttp_reset( &parser );
            enum llhttp_errno err = llhttp_execute( &parser, res.c_str( ), res.size( ) );
            if ( err == HPE_OK )
                return response;
            else
                throw std::runtime_error(
                    std::format( "failed to parse HTTP request: {} {}", llhttp_errno_name( err ), parser.reason ) );
        }

    private:
        llhttp_settings_t settings;
    };
} // namespace vrock::http
