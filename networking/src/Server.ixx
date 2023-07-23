module;

#include <cstdint>
#include <iostream>
#include <queue>
#include <thread>

export module vrock.networking.Server;
import vrock.networking.Sockets;

namespace vrock::networking
{
    export class BaseServer
    {
    public:
        BaseServer( Domain domain, Type type, std::uint16_t port, int backlog,
                    std::size_t max_threads = std::thread::hardware_concurrency( ) / 2,
                    unsigned int interface = any_ip )
            : domain( domain ), type( type ), port( port ), backlog( backlog ), max_threads( max_threads ),
              interface( interface ), socket( nullptr )
        {
        }

        auto run( ) -> void
        {
            running_ = true;
            socket = std::make_unique<ListeningSocket>( domain, type, port, backlog, interface );
            worker_queue.resize( max_threads );

            worker_threads.reserve( max_threads );
            for ( int i = 0; i < max_threads; ++i )
                worker_threads[ i ] = std::thread( &BaseServer::work, this, i );
            listener_thread_ = std::thread( &BaseServer::listen, this );
        }

        auto stop( ) -> void
        {
        }

        auto work( std::size_t id ) -> void
        {
            auto &queue = worker_queue[ id ];
            bool active = false;
            while ( running_ )
            {
                if ( !active )
                    std::this_thread::sleep_for( std::chrono::microseconds( 100 ) );

                if ( queue.empty( ) )
                {
                    active = false;
                    continue;
                }
                active = true;

                auto data = queue.front( ).receive( );
                handle_request( queue.front( ), data );

                queue.pop( );
            }
        }

        auto listen( ) -> void
        {
            std::size_t current_worker = 0;
            while ( running_ )
            {
                worker_queue[ current_worker ].emplace( socket->accept( ) );
                current_worker = ++current_worker % max_threads;
            }
        }

        auto handle_request( Socket &request_origin, const utils::ByteArray<> &data ) -> void
        {
            std::cout << data.to_string( ) << std::endl;
            request_origin.send( utils::ByteArray<>( "Hello" ) );
        }

    protected:
        // Server stuff
        bool running_ = false;

        // Socket stuff
        Domain domain;
        Type type;
        std::uint16_t port;
        int backlog;
        std::size_t max_threads;
        unsigned int interface;
        std::unique_ptr<ListeningSocket> socket = nullptr;

        // thread stuff
        std::thread listener_thread_;
        std::vector<std::thread> worker_threads = { };
        std::vector<std::queue<Socket>> worker_queue = { };
    };
} // namespace vrock::networking