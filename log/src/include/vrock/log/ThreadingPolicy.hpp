#pragma once
#include "Logger.hpp"

namespace vrock::log
{
    /**
     * @brief A lightweight null mutex structure for single-threaded policies.
     *
     * The NullMutex structure provides a simple null mutex implementation for single-threaded logging policies.
     * It includes static lock and unlock methods that do nothing, making it suitable for use in scenarios where
     * synchronization is not required.
     */
    struct NullMutex
    {
        /**
         * @brief Dummy lock operation (no-op) for compatibility with mutex interfaces.
         */
        static void lock( )
        {
        }

        /**
         * @brief Dummy unlock operation (no-op) for compatibility with mutex interfaces.
         */
        static void unlock( )
        {
        }
    };

    /**
     * @brief Logging policy for single-threaded scenarios.
     *
     * The SingleThreadedPolicy structure defines a logging policy tailored for single-threaded environments.
     * It utilizes the NullMutex structure to provide a lightweight, non-blocking mutex. The 'mutex_t' type
     * alias is set to NullMutex, and 'lock' is defined as a std::lock_guard using NullMutex.
     */
    struct SingleThreadedPolicy
    {
        using mutex_t = NullMutex;               /**< Type alias for the mutex type in the single-threaded policy. */
        using lock = std::lock_guard<NullMutex>; /**< Type alias for the lock type in the single-threaded policy. */
    };

    /**
     * @brief Logging policy for multi-threaded scenarios.
     *
     * The MultiThreadedPolicy structure defines a logging policy suitable for multi-threaded environments.
     * It uses std::mutex for thread synchronization. The 'mutex_t' type alias is set to std::mutex, and 'lock'
     * is defined as a std::lock_guard using std::mutex.
     */
    struct MultiThreadedPolicy
    {
        using mutex_t = std::mutex;               /**< Type alias for the mutex type in the multi-threaded policy. */
        using lock = std::lock_guard<std::mutex>; /**< Type alias for the lock type in the multi-threaded policy. */
    };

} // namespace vrock::log