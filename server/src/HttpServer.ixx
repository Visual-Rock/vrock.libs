module;

#include <sys/epoll.h>

#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

export module vrock.server.HttpServer;

export import vrock.server.HttpMessage;

namespace vrock::server
{
    export constexpr std::size_t max_connections = 10000;
    export constexpr std::size_t max_events = 10000;
    export constexpr std::size_t backlog_size = 1000;
    export constexpr std::size_t max_buffer_size = 4096;

    struct EventData
    {
        int fd = 0;
        size_t len = 0;
        size_t cursor = 0;
        char buffer[ max_buffer_size ]{ };
    };

    export class HttpServer
    {
    public:
        HttpServer( const std::string &host, std::uint16_t port,
                    std::size_t max_threads = std::thread::hardware_concurrency( ) / 2 );
        ~HttpServer( );

        auto add_route( const std::string &route, HttpMethod method,
                        std::function<HttpResponse( const HttpRequest & )> handler ) -> void
        {
            routes[ route ][ method ] = std::move( handler );
        }

        auto run( ) -> void;
        auto stop( ) -> void;

        auto inline host( ) const noexcept -> std::string
        {
            return host_;
        }
        auto inline port( ) const noexcept -> std::uint16_t
        {
            return port_;
        }
        auto inline threads( ) const noexcept -> std::size_t
        {
            return max_threads_;
        }
        auto inline running( ) const noexcept -> bool
        {
            return running_;
        }

    private:
        std::string host_;
        std::uint16_t port_;
        std::size_t max_threads_;
        bool running_;

        std::unordered_map<std::string,
                           std::unordered_map<HttpMethod, std::function<HttpResponse( const HttpRequest & )>>>
            routes;

    private:
        int sock = 0;
        std::vector<int> worker_epoll_;
        std::thread listener_thread_;
        std::vector<std::thread> worker_threads_;
        epoll_event **worker_events_;

        auto create_socket( ) -> void;
        auto listen_( ) -> void;
        auto process_events( size_t ) -> void;
    };
} // namespace vrock::server