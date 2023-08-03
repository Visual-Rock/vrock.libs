module;

#include "openssl/err.h"
#include "openssl/ssl.h"
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
    class SecureSocket : public vrock::networking::Socket
    {
    public:
        SecureSocket( SSL *ssl ) : vrock::networking::Socket( SSL_get_fd( ssl ) ), ssl( ssl )
        {
        }

        auto close( ) const -> void final
        {
            ::close( socket_ );
            SSL_free( ssl );
        }

        auto send( const utils::ByteArray<> &data, int flags = 0 ) const -> void final
        {
            ssize_t data_send = 0;
            int ret;
            while ( data_send < data.size( ) )
            {
                ret = SSL_write( ssl, data.data( ) + data_send, data.size( ) - data_send );
                if ( ret == -1 )
                    throw std::runtime_error( "send failed" );
                data_send += ret;
            }
        }

        [[nodiscard]] auto receive( int flags = 0 ) const -> utils::ByteArray<> final
        {
            utils::List<utils::ByteArray<>> data = { };
            ssize_t data_recv = 0;
            std::size_t i = 0;
            while ( true )
            {
                data.emplace_back( 4096 );
                auto r = SSL_read( ssl, data[ i ].data( ), data[ i ].size( ) );
                if ( r == -1 && !( SSL_get_error( ssl, r ) == 5 && strcmp( strerror( errno ), "Successes" ) != 0 ) )
                    throw std::runtime_error(
                        std::format( "receive failed {} {}", SSL_get_error( ssl, r ), strerror( errno ) ) );
                else if ( r == 0 )
                    return { };
                data_recv += r;
                if ( r < 4096 )
                    return utils::combine_arrays( data, data_recv ); // finished reading
                ++i;
            }
        }

    private:
        SSL *ssl;
    };

    struct SocketData
    {
        int fd;
        SSL *ssl;
    };

    export template <std::size_t MaxEvents = 10000, std::size_t BacklogSize = 1000>
    class TCPServer
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
            if ( use_ssl )
            {
                SSL_library_init( );
                ssl_ctx = create_ctx( );
                load_certs( );
            }
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
            SSL_CTX_free( ssl_ctx );
        }

        auto set_certificate( std::string cert, std::string key ) -> void
        {
            use_ssl = true;
            cert_file = cert;
            key_file = key;
        }

        auto get_host( ) -> std::string
        {
            return host;
        }

        auto get_port( ) -> std::uint16_t
        {
            return port;
        }

        auto get_max_threads( ) -> std::size_t
        {
            return max_threads;
        }

    protected:
        virtual auto handle_request( const vrock::utils::ByteArray<> &data,
                                     std::function<void( const utils::ByteArray<> & )> send ) -> bool = 0;

    private:
        auto listen( ) -> void
        {
            sockaddr_in client_address{ };
            socklen_t client_len = sizeof( client_address );
            int client_fd;
            int current_worker = 0;
            bool active = true;
            SocketData *data;

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
                data = new SocketData( );
                data->fd = client_fd;
                if ( use_ssl )
                {
                    data->ssl = SSL_new( ssl_ctx );
                    SSL_set_fd( data->ssl, client_fd );
                    int i = 0;
                    while ( SSL_accept( data->ssl ) == -1 && i != 100 )
                    {
                        std::this_thread::sleep_for( std::chrono::microseconds( 100 ) );
                        i++;
                    }
                    // if ( SSL_accept( data->ssl ) == -1 && SSL_accept( data->ssl ) == -1 )
                    /*{
                        ::close( data->fd );
                        SSL_free( data->ssl );
                        delete data;
                        std::cout << "SSL_accept failed" << std::endl;
                        continue;
                    }*/
                }
                ctl_epoll_ev( worker_epoll[ current_worker ], EPOLL_CTL_ADD, client_fd, data, EPOLLIN );

                current_worker = ++current_worker % max_threads;
            }
        }

        auto process_events( std::size_t id ) -> void
        {
            int epoll_fd = worker_epoll[ id ];
            bool active = true;
            SocketData *data;

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
                    data = (SocketData *)current_event.data.ptr;
                    if ( ( current_event.events & EPOLLHUP ) || ( current_event.events & EPOLLERR ) )
                    {
                        ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, data->fd );
                        close( data->fd );
                        if ( use_ssl && data->ssl )
                            SSL_free( data->ssl );
                        delete data;
                    }
                    else if ( current_event.events == EPOLLIN || current_event.events == EPOLLOUT )
                    {
                        try
                        {
                            if ( use_ssl )
                            {
                                auto sock = SecureSocket( data->ssl );
                                auto d = sock.receive( );
                                if ( handle_request( d,
                                                     [ & ]( const utils::ByteArray<> &data ) { sock.send( data ); } ) )
                                {
                                    ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, data->fd );
                                    close( data->fd );
                                    if ( use_ssl && data->ssl )
                                        SSL_free( data->ssl );
                                    delete data;
                                }
                            }
                            else
                            {
                                auto sock = Socket( data->fd );
                                auto d = sock.receive( );
                                if ( handle_request( d,
                                                     [ & ]( const utils::ByteArray<> &data ) { sock.send( data ); } ) )
                                {
                                    ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, data->fd );
                                    close( data->fd );
                                    delete data;
                                }
                            }
                        }
                        catch ( std::exception &e )
                        {
                            std::cout << std::format( "exception: {}", e.what( ) ) << std::endl;
                            ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, data->fd );
                            close( data->fd );
                            if ( use_ssl && data->ssl )
                                SSL_free( data->ssl );
                            delete data;
                        }
                    }
                    else
                    { // something unexpected
                        ctl_epoll_ev( epoll_fd, EPOLL_CTL_DEL, data->fd );
                        close( data->fd );
                        if ( use_ssl && data->ssl )
                            SSL_free( data->ssl );
                        delete data;
                    }
                }
            }
        }

        auto ctl_epoll_ev( int epoll_fd, int op, int fd, void *data = nullptr, std::uint32_t events = 0 ) -> void
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
                ev.data.ptr = data;
                if ( epoll_ctl( epoll_fd, op, fd, &ev ) < 0 )
                    throw std::runtime_error( "Failed to add file descriptor" );
            }
        }

        auto create_ctx( ) -> SSL_CTX *
        {
            SSL_CTX *ctx;
            OpenSSL_add_all_algorithms( );                   /* load & register all cryptos, etc. */
            SSL_load_error_strings( );                       /* load all error messages */
            const SSL_METHOD *method = TLS_server_method( ); /* create new server-method instance */
            ctx = SSL_CTX_new( method );                     /* create new context from method */
            if ( ctx == NULL )
            {
                ERR_print_errors_fp( stderr );
                throw std::runtime_error( "" );
            }
            return ctx;
        }

        auto load_certs( ) -> void
        {
            /* set the local certificate from CertFile */
            if ( SSL_CTX_use_certificate_file( ssl_ctx, cert_file.c_str( ), SSL_FILETYPE_PEM ) <= 0 )
            {
                ERR_print_errors_fp( stderr );
                throw std::runtime_error( "" );
            }
            /* set the private key from KeyFile (may be the same as CertFile) */
            if ( SSL_CTX_use_PrivateKey_file( ssl_ctx, key_file.c_str( ), SSL_FILETYPE_PEM ) <= 0 )
            {
                ERR_print_errors_fp( stderr );
                throw std::runtime_error( "" );
            }
            /* verify private key */
            if ( !SSL_CTX_check_private_key( ssl_ctx ) )
                throw std::runtime_error( "Private key does not match the public certificate" );
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

        bool use_ssl = false;
        SSL_CTX *ssl_ctx;
        std::string cert_file;
        std::string key_file;
    };
} // namespace vrock::http