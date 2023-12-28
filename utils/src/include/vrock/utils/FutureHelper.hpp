#pragma once

#include <chrono>
#include <future>

namespace vrock::utils
{
    /**
     * @brief Checks if the provided std::future is ready within a specified duration.
     * @param future The std::future to be checked.
     * @param duration The maximum duration to wait for the future to be ready.
     * @return `true` if the future is ready, `false` otherwise or if the future is invalid.
     */
    template <class T>
    auto future_ready( const std::future<T> &future,
                       std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::ready;
        return false;
    }

    /**
     * @brief Checks if the provided std::shared_future is ready within a specified duration.
     * @param future The std::shared_future to be checked.
     * @param duration The maximum duration to wait for the future to be ready.
     * @return `true` if the future is ready, `false` otherwise or if the future is invalid.
     */
    template <class T>
    auto future_ready( const std::shared_future<T> &future,
                       std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::ready;
        return false;
    }

    /**
     * @brief Checks if the provided std::future has timed out within a specified duration.
     * @param future The std::future to be checked.
     * @param duration The maximum duration to wait for the future to complete or time out.
     * @return `true` if the future has timed out, `false` otherwise or if the future is invalid.
     */
    template <class T>
    auto future_timeout( const std::future<T> &future,
                         std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::timeout;
        return false;
    }

    /**
     * @brief Checks if the provided std::shared_future has timed out within a specified duration.
     * @param future The std::shared_future to be checked.
     * @param duration The maximum duration to wait for the future to complete or time out.
     * @return `true` if the future has timed out, `false` otherwise or if the future is invalid.
     */
    template <class T>
    auto future_timeout( const std::shared_future<T> &future,
                         std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::timeout;
        return false;
    }

    /**
     * @brief Checks if the provided std::future has been deferred within a specified duration.
     * @param future The std::future to be checked.
     * @param duration The maximum duration to wait for the future to be deferred.
     * @return `true` if the future has been deferred, `false` otherwise or if the future is invalid.
     */
    template <class T>
    auto future_deferred( const std::future<T> &future,
                          std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::deferred;
        return false;
    }

    /**
     * @brief Checks if the provided std::shared_future has been deferred within a specified duration.
     * @param future The std::shared_future to be checked.
     * @param duration The maximum duration to wait for the future to be deferred.
     * @return `true` if the future has been deferred, `false` otherwise or if the future is invalid.
     */
    template <class T>
    auto future_deferred( const std::shared_future<T> &future,
                          std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) ) -> bool
    {
        if ( future.valid( ) )
            return future.wait_for( duration ) == std::future_status::deferred;
        return false;
    }
} // namespace vrock::utils
