#pragma once

#include <chrono>
#include <future>

namespace vrock::utils
{
    template <class T>
    auto future_ready( const std::future<T> &future,
                       std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::ready;
        return false;
    }

    template <class T>
    auto future_ready( const std::shared_future<T> &future,
                       std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::ready;
        return false;
    }

    template <class T>
    auto future_timeout( const std::future<T> &future,
                         std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::timeout;
        return false;
    }

    template <class T>
    auto future_timeout( const std::shared_future<T> &future,
                         std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::timeout;
        return false;
    }

    template <class T>
    auto future_deferred( const std::future<T> &future,
                          std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::deferred;
        return false;
    }

    template <class T>
    auto future_deferred( const std::shared_future<T> &future,
                          std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::timeout;
        return false;
    }
} // namespace vrock::utils