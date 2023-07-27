module;

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <cstring>
#include <format>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <string>
#include <thread>
#include <unordered_map>

module vrock.server.HttpServer;

namespace vrock::server
{
    auto handle_http_data( const EventData &raw_request, EventData *raw_response ) -> void;

    auto epoll_event_( int epoll_fd, int op, int fd, std::uint32_t events = 0, void *data = nullptr ) -> void;
    auto handle_epoll_event( int fd, EventData *data, std::uint32_t event ) -> void;

    HttpServer::HttpServer( const std::string &host, std::uint16_t port, std::size_t max_threads )
        : host_( host ), port_( port ), max_threads_( max_threads ), running_( false )
    {
        worker_events_ = (epoll_event **)( std::malloc( sizeof( epoll_event * ) * max_threads_ ) );
        for ( int i = 0; i < max_threads_; ++i )
            worker_events_[ i ] = (epoll_event *)std::malloc( sizeof( epoll_event ) * max_events );
        create_socket( );
    }

    HttpServer::~HttpServer( )
    {
        if ( running_ )
            stop( );
        if ( worker_events_ )
            for ( int i = 0; i < max_threads_; ++i )
                delete[] worker_events_[ i ];
        delete[] worker_events_;
    }

    auto HttpServer::run( ) -> void
    {
        // do socket things
        int opt = 1;
        sockaddr_in server_address{ };

        if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ) ) < 0 )
            throw std::runtime_error( "failed to set socket options" );
        struct addrinfo hints;
        struct addrinfo *servinfo; // will point to the results

        memset( &hints, 0, sizeof hints ); // make sure the struct is empty
        hints.ai_family = AF_UNSPEC;       // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;   // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;       // fill in my IP for me
        getaddrinfo( "0.0.0.0", "8080", &hints, &servinfo );
        //        server_address.sin_family = AF_INET;
        //        server_address.sin_addr.s_addr = INADDR_ANY;
        //        inet_pton( AF_INET, host_.c_str( ), &( server_address.sin_addr.s_addr ) );
        //        server_address.sin_port = htons( port_ );
        // (sockaddr *)&server_address sizeof( server_address )
        if ( bind( sock, servinfo->ai_addr, servinfo->ai_addrlen ) < 0 )
            throw std::runtime_error( "failed to bind to socket" );
        if ( listen( sock, backlog_size ) < 0 )
            throw std::runtime_error( std::format( "failed to listen on port {}", port_ ) );
        freeaddrinfo( servinfo );
        // epoll
        worker_epoll_.resize( max_threads_ );
        for ( size_t i = 0; i < max_threads_; ++i )
            if ( ( worker_epoll_[ i ] = epoll_create1( 0 ) ) < 0 )
                throw std::runtime_error( "failed to create epoll for worker" );

        // setup worker threads
        running_ = true;
        listener_thread_ = std::thread( &HttpServer::listen_, this );
        worker_threads_.resize( max_threads_ );
        for ( size_t i = 0; i < max_threads_; ++i )
            worker_threads_[ i ] = std::thread( &HttpServer::process_events, this, i );
    }

    auto HttpServer::stop( ) -> void
    {
        running_ = false;
        listener_thread_.join( );
        for ( auto &thread : worker_threads_ )
            thread.join( );
        for ( auto fd : worker_epoll_ )
            close( fd );
        close( sock );
    }

    auto HttpServer::listen_( ) -> void
    {
        EventData *data;
        sockaddr_in client_addr;
        socklen_t client_len = sizeof( client_addr );
        int client_fd;
        std::size_t current_worker = 0;
        bool active = true;
        while ( running_ )
        {
            if ( !active )
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
            client_fd = accept4( sock, (sockaddr *)&client_addr, &client_len, SOCK_NONBLOCK );
            // continue if no request happened
            if ( client_fd < 0 )
            {
                active = false;
                continue;
            }

            active = true;
            data = new EventData( );
            data->fd = client_fd;

            epoll_event_( worker_epoll_[ current_worker ], EPOLL_CTL_ADD, client_fd, EPOLLIN, data );
            current_worker = ++current_worker % max_threads_;
        }
    }

    auto HttpServer::process_events( size_t id ) -> void
    {
        EventData *data;
        int epoll_fd = worker_epoll_[ id ];
        bool active = true;
        while ( running_ )
        {
            if ( !active )
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

            int new_fds = epoll_wait( epoll_fd, worker_events_[ id ], max_events, 0 );
            if ( new_fds <= 0 )
            {
                active = false;
                continue;
            }

            active = true;
            for ( int i = 0; i < new_fds; ++i )
            {
                const epoll_event &current_event = worker_events_[ id ][ i ];
                data = reinterpret_cast<EventData *>( current_event.data.ptr );
                // EPOLLHUP || EPOLLERR || something unexpected
                if ( ( current_event.events & EPOLLHUP ) || ( current_event.events & EPOLLERR ) )
                {
                    epoll_event_( epoll_fd, EPOLL_CTL_DEL, data->fd );
                    close( data->fd );
                    delete data;
                }
                else if ( ( current_event.events == EPOLLIN ) || ( current_event.events == EPOLLOUT ) )
                {
                    handle_epoll_event( epoll_fd, data, current_event.events );
                }
                else
                { // something unexpected
                    epoll_event_( epoll_fd, EPOLL_CTL_DEL, data->fd );
                    close( data->fd );
                    delete data;
                }
            }
        }
    }

    auto HttpServer::create_socket( ) -> void
    {
        if ( ( sock = socket( AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0 ) ) < 0 )
            throw std::runtime_error( "failed to create socket" );
    }

    auto epoll_event_( int epoll_fd, int op, int fd, std::uint32_t events, void *data ) -> void
    {
        if ( op == EPOLL_CTL_DEL )
        {
            if ( epoll_ctl( epoll_fd, op, fd, nullptr ) < 0 )
                throw std::runtime_error( "failed to remove file descriptor" );
        }
        else
        {
            epoll_event ev{ };
            ev.events = events;
            ev.data.ptr = data;
            if ( epoll_ctl( epoll_fd, op, fd, &ev ) < 0 )
                throw std::runtime_error( "failed to add file descriptor" );
        }
    }

    auto handle_epoll_event( int epoll_fd, EventData *data, std::uint32_t events ) -> void
    {
        int fd = data->fd;
        EventData *request, *response;

        if ( events == EPOLLIN )
        {
            request = data;
            ssize_t byte_count = recv( fd, request->buffer, max_buffer_size, 0 );
            if ( byte_count > 0 )
            {
                // fully received the message
                response = new EventData( );
                response->fd = fd;
                handle_http_data( *request, response );
                epoll_event_( epoll_fd, EPOLL_CTL_MOD, fd, EPOLLOUT, response );
                delete request;
            }
            else if ( byte_count == 0 )
            {
                // client closed connection
                epoll_event_( epoll_fd, EPOLL_CTL_DEL, fd );
                close( fd );
                delete request;
            }
            else
            {
                if ( errno == EAGAIN || errno == EWOULDBLOCK )
                {
                    // retry
                    request->fd = fd;
                    epoll_event_( epoll_fd, EPOLL_CTL_MOD, fd, EPOLLIN, request );
                }
                else
                {
                    // error
                    epoll_event_( epoll_fd, EPOLL_CTL_DEL, fd );
                    close( fd );
                    delete request;
                }
            }
        }
        else
        {
            response = data;
            ssize_t byte_count = send( fd, response->buffer + response->cursor, response->len, 0 );
            if ( byte_count >= 0 )
            {
                if ( byte_count < response->len )
                {
                    // bytes left to write
                    response->cursor += byte_count;
                    response->len -= byte_count;
                    epoll_event_( epoll_fd, EPOLL_CTL_MOD, fd, EPOLLOUT, response );
                }
                else
                {
                    // complete message written
                    request = new EventData( );
                    request->fd = fd;
                    epoll_event_( epoll_fd, EPOLL_CTL_MOD, fd, EPOLLIN, request );
                    delete response;
                    // delete request;
                    close( fd );
                }
            }
            else
            {
                if ( errno == EAGAIN || errno == EWOULDBLOCK )
                {
                    // retry
                    epoll_event_( epoll_fd, EPOLL_CTL_ADD, fd, EPOLLOUT, response );
                }
                else
                {
                    // other error
                    epoll_event_( epoll_fd, EPOLL_CTL_DEL, fd );
                    close( fd );
                    delete response;
                }
            }
        }
    }

    auto handle_http_data( const EventData &raw_request, EventData *raw_response ) -> void
    {
        std::string request_string( raw_request.buffer ), response_string;
        HttpRequest request;
        HttpResponse response;

        // std::cout << request_string << std::endl;

        response_string = "HTTP/1.1 200 OK\r\n"
                          "Server: Hello\r\n"
                          "Content-Length: 13\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "Hello, world\r\n";
        std::memcpy( raw_response->buffer, response_string.data( ), max_buffer_size );
        raw_response->len = response_string.size( );
    }
} // namespace vrock::server