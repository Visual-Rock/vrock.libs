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
#include <utility>

export module vrock.http.TCPServer;

import vrock.networking.Sockets;
import vrock.utils.ByteArray;

using Socket = vrock::networking::Socket;

namespace vrock::http
{
    export template <std::size_t MaxEvents = 10000, std::size_t BacklogSize = 1000> class TCPServer
    {
    public:
        TCPServer( std::string host, std::uint16_t port,
                   std::size_t max_threads = std::max( 1u, std::thread::hardware_concurrency( ) / 2 ) )
            : host( std::move( host ) ), port( port ), max_threads( max_threads )
        {
            worker_events = (epoll_event **)( std::malloc( sizeof( epoll_event * ) * max_threads ) );
            for ( int i = 0; i < max_threads; ++i )
            {
                worker_events[ i ] = (epoll_event *)std::malloc( sizeof( epoll_event ) * MaxEvents );
                std::memset( worker_events[ i ], 0, sizeof( epoll_event ) * MaxEvents );
            }
            if ( ( socket_ = ::socket( AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0 ) ) < 0 )
                throw std::runtime_error( "Failed to create a TCP socket" );
        }

        ~TCPServer( )
        {
            if ( running )
                stop( );
            if ( worker_events )
            {
                for ( int i = 0; i < max_threads; ++i )
                    free( worker_events[ i ] );
            }
            free( worker_events );
        }

        auto run( ) -> void
        {
            // do socket things
            int opt = 1;
            if ( setsockopt( socket_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ) ) < 0 )
                throw std::runtime_error( "failed to set socket options" );

            struct addrinfo hints
            {
            };
            struct addrinfo *servinfo;         // will point to the results
            memset( &hints, 0, sizeof hints ); // make sure the struct is empty
            hints.ai_family = AF_UNSPEC;       // don't care IPv4 or IPv6
            hints.ai_socktype = SOCK_STREAM;   // TCP stream sockets
            hints.ai_flags = AI_PASSIVE;       // fill in my IP for me
            if ( getaddrinfo( "0.0.0.0", "8080", &hints, &servinfo ) != 0 )
                throw std::runtime_error( "getaddrinfo failed" );

            if ( bind( socket_, servinfo->ai_addr, servinfo->ai_addrlen ) < 0 )
                throw std::runtime_error( "failed to bind to socket" );

            if ( ::listen( socket_, BacklogSize ) < 0 )
                throw std::runtime_error( std::format( "failed to listen on port {}", port ) );
            freeaddrinfo( servinfo );
            // epoll
            worker_epoll.resize( max_threads );
            for ( size_t i = 0; i < max_threads; ++i )
                if ( ( worker_epoll[ i ] = epoll_create1( 0 ) ) < 0 )
                    throw std::runtime_error( "failed to create epoll for worker" );

            // setup worker threads
            running = true;
            listener_thread = std::thread( &TCPServer::listen, this );
            worker_threads.resize( max_threads );
            for ( size_t i = 0; i < max_threads; ++i )
                worker_threads[ i ] = std::thread( &TCPServer::process_events, this, i );
        }

        auto stop( ) -> void
        {
            running = false;
            listener_thread.join( );
            for ( auto &thread : worker_threads )
                thread.join( );
            for ( int i = 0; i < max_threads; i++ )
            {
                close( worker_epoll[ i ] );
            }
            close( socket_ );
        }

    protected:
        virtual auto handle_request( const Socket &socket, const vrock::utils::ByteArray<> &data ) -> bool = 0;

    private:
        auto listen( ) -> void
        {
            sockaddr_in client_address{ };
            socklen_t client_len = sizeof( client_address );
            int client_fd;
            int current_worker = 0;
            bool active = true;

            while ( running )
            {
                if ( !active )
                    std::this_thread::sleep_for( std::chrono::microseconds( 50 ) );
                client_fd = accept4( socket_, (sockaddr *)&client_address, &client_len, SOCK_NONBLOCK );

                if ( client_fd < 0 )
                {
                    active = false;
                    continue;
                }

                active = true;

                ctl_epoll_ev( worker_epoll[ current_worker ], EPOLL_CTL_ADD, client_fd, EPOLLIN );

                current_worker = ++current_worker % max_threads;
            }
        }

        auto process_events( std::size_t id ) -> void
        {
            int epoll_fd = worker_epoll[ id ];
            bool active = true;

            while ( running )
            {
                if ( !active )
                    std::this_thread::sleep_for( std::chrono::microseconds( 50 ) );

                auto nfds = epoll_wait( epoll_fd, worker_events[ id ], MaxEvents, 0 );

                if ( nfds <= 0 )
                {
                    active = false;
                    continue;
                }
                active = true;
                for ( int i = 0; i < nfds; i++ )
                {
                    const epoll_event &current_event = worker_events[ id ][ i ];
                    if ( ( current_event.events & EPOLLHUP ) || ( current_event.events & EPOLLERR ) )
                    {
                        ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, current_event.data.fd );
                        close( current_event.data.fd );
                    }
                    else if ( current_event.events == EPOLLIN || current_event.events == EPOLLOUT )
                    {
                        Socket sock( current_event.data.fd );
                        try
                        {
                            auto d = sock.receive( );
                            if ( handle_request( sock, d ) )
                            {
                                ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, current_event.data.fd );
                                sock.close( );
                            }
                        }
                        catch ( std::exception &e )
                        {
                            std::cout << std::format( "exception: {}", e.what( ) ) << std::endl;
                            ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, current_event.data.fd );
                            close( current_event.data.fd );
                        }
                    }
                    else
                    { // something unexpected
                        ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, current_event.data.fd );
                        close( current_event.data.fd );
                    }
                }
            }
        }

        auto ctl_epoll_ev( int epoll_fd, int op, int fd, std::uint32_t events = 0 ) -> void
        {
            if ( op == EPOLL_CTL_DEL )
            {
                if ( epoll_ctl( epoll_fd, op, fd, nullptr ) < 0 )
                    throw std::runtime_error( "Failed to remove file descriptor" );
            }
            else
            {
                epoll_event ev{ };
                ev.events = events;
                ev.data.fd = fd;
                if ( epoll_ctl( epoll_fd, op, fd, &ev ) < 0 )
                    throw std::runtime_error( "Failed to add file descriptor" );
            }
        }

    private:
        std::string host;
        std::uint16_t port;
        std::size_t max_threads;
        bool running = false;

        std::thread listener_thread;
        std::vector<std::thread> worker_threads;

        int socket_;
        std::vector<int> worker_epoll;
        epoll_event **worker_events;
    };
} // namespace vrock::http