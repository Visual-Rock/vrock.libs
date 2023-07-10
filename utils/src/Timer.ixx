module;

#include <chrono>
#include <functional>

export module vrock.utils.Timer;

namespace vrock::utils
{
    export class Timer
    {
    public:
        Timer( ) : begin( std::chrono::high_resolution_clock::now( ) )
        {
        }

        auto reset( ) -> void
        {
            begin = std::chrono::high_resolution_clock::now( );
        }

        template <class T = std::chrono::milliseconds>
        auto elapsed( ) -> std::uint64_t
        {
            return std::chrono::duration_cast<T>( std::chrono::high_resolution_clock::now( ) - begin ).count( );
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    };

    export template <class T = std::chrono::milliseconds>
    class ScopedTimer
    {
    public:
        explicit ScopedTimer( std::function<void( std::uint64_t )> fn ) : timer( { } ), fn( std::move( fn ) )
        {
        }
        ~ScopedTimer( )
        {
            fn( elapsed( ) );
        }

        auto elapsed( ) -> std::uint64_t
        {
            return timer.elapsed<T>( );
        }

    private:
        Timer timer;
        std::function<void( std::uint64_t )> fn;
    };
} // namespace vrock::utils