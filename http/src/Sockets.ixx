module;

#include <cstdint>
#include <stdexcept>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

export module vrock.networking.Sockets;

export import vrock.utils.ByteArray;
export import vrock.utils.List;

namespace vrock::networking
{
    export constexpr auto any_ip = INADDR_ANY;

    export enum class Domain
    {
        IPv4 = AF_INET
    };

    auto get_address( Domain domain, std::uint16_t port, unsigned int interface = any_ip ) -> sockaddr_in
    {
        sockaddr_in address{ };
        address.sin_family = (int)domain;
        address.sin_port = htons( port );
        address.sin_addr.s_addr = htonl( interface );
        return address;
    }

    export enum class Type
    {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM,

        NonBlocking = SOCK_NONBLOCK
    };

    export class Socket
    {
    public:
        Socket( Domain domain, Type type )
        {
            socket_ = socket( (int)domain, (int)type, 0 );
            if ( socket_ == -1 ) // error
                throw std::runtime_error( "failed to create socket" );
        }

        explicit Socket( int fd ) : socket_( fd )
        {
        }

        ~Socket( ) = default;

        [[nodiscard]] auto inline get_socket( ) const noexcept -> int
        {
            return socket_;
        }

        auto close( ) const -> void
        {
            ::close( socket_ );
        }

        auto send( const utils::ByteArray<> &data, int flags = 0 ) const -> void
        {
            ssize_t data_send = 0;
            int ret;
            while ( data_send < data.size( ) )
            {
                ret = ::send( socket_, data.data( ) + data_send, data.size( ) - data_send, flags );
                if ( ret == -1 )
                    throw std::runtime_error( "send failed" );
                data_send += ret;
            }
        }

        [[nodiscard]] auto receive( int flags = 0 ) const -> utils::ByteArray<>
        {
            utils::List<utils::ByteArray<>> data = { };
            ssize_t data_recv = 0;
            std::size_t i = 0;
            while ( true )
            {
                data.emplace_back( 4096 );
                auto r = ::recv( socket_, data[ i ].data( ), data[ i ].size( ), flags );
                if ( r == -1 )
                    throw std::runtime_error( "receive failed" );
                else if ( r == 0 )
                    return { };
                data_recv += r;
                if ( r < 4096 )
                    return utils::combine_arrays( data, data_recv ); // finished reading
                ++i;
            }
        }

    protected:
        int socket_;
    };

    export class BindingSocket : public Socket
    {
    public:
        BindingSocket( Domain domain, Type type, std::uint16_t port, unsigned int interface = any_ip )
            : Socket( domain, type )
        {
            auto addr = get_address( domain, port, interface );
            if ( bind( socket_, (sockaddr *)&addr, sizeof( addr ) ) != 0 )
                throw std::runtime_error( "failed to bind socket" );
        }
    };

    export class ListeningSocket : public BindingSocket
    {
    public:
        ListeningSocket( Domain domain, Type type, std::uint16_t port, int backlog, unsigned int interface = any_ip )
            : BindingSocket( domain, type, port, interface )
        {
            if ( listen( socket_, backlog ) == -1 )
                throw std::runtime_error( "failed to setup listener" );
        }

        auto accept( ) -> Socket
        {
            struct sockaddr_storage addr
            {
            };
            socklen_t addr_size = sizeof( addr );
            auto fd = ::accept( socket_, (struct sockaddr *)&addr, &addr_size );
            if ( fd == -1 )
                throw std::runtime_error( "failed to accept incoming" );
            return Socket( fd );
        }
    };

    export class ConnectingSocket : public Socket
    {
    public:
        ConnectingSocket( Domain domain, Type type, const std::string &address, std::uint16_t port )
            : Socket( domain, type )
        {
            struct addrinfo hints
            {
            }, *res;
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = (int)domain;
            if ( getaddrinfo( address.c_str( ), std::to_string( port ).c_str( ), &hints, &res ) != 0 )
                throw std::runtime_error( "" );
            if ( connect( socket_, res->ai_addr, res->ai_addrlen ) != 0 )
                throw std::runtime_error( "failed to connect socket" );
        }
    };
} // namespace vrock::networking