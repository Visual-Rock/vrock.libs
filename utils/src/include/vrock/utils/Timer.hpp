#pragma once

#include <chrono>
#include <functional>

namespace vrock::utils
{
    /**
     * @class Timer
     *
     * `Timer` is a class to measure time
     */
    class Timer
    {
    public:
        /**
         * @brief starts the internal timer
         */
        Timer( ) : begin( std::chrono::high_resolution_clock::now( ) )
        {
        }

        /**
         * @brief resets the intrnal timer
         */
        auto reset( ) -> void
        {
            begin = std::chrono::high_resolution_clock::now( );
        }

        /**
         * @brief gets the elapsed time
         * @tparam T resolution of the returned value, default in milliseconds
         * @return time elapsed since timer start
         */
        template <class T = std::chrono::milliseconds>
        auto elapsed( ) -> std::uint64_t
        {
            return std::chrono::duration_cast<T>( std::chrono::high_resolution_clock::now( ) - begin ).count( );
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    };

    /**
     * @class ScopedTimer
     *
     * `ScopedTimer` is a class to measure time in a scope. it executes a given predicate on destruction
     * @tparam T resolution of the timer. default is milliseconds
     */
    template <class T = std::chrono::milliseconds>
    class ScopedTimer
    {
    public:
        /**
         * @brief starts the internal timer
         * @param fn function to execute on destruction
         */
        explicit ScopedTimer( std::function<void( std::uint64_t )> fn ) : timer( { } ), fn( std::move( fn ) )
        {
        }
        /**
         * @brief executes the given function
         */
        ~ScopedTimer( )
        {
            fn( elapsed( ) );
        }

        /**
         * @brief gets the elapsed time
         * @return time elapsed since timer start
         */
        auto elapsed( ) -> std::uint64_t
        {
            return timer.elapsed<T>( );
        }

    private:
        Timer timer;
        std::function<void( std::uint64_t )> fn;
    };
} // namespace vrock::utils
