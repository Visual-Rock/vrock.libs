#pragma once
#include "Logger.hpp"

namespace vrock::log
{
    struct NullMutex
    {
        static void lock( )
        {
        }

        static void unlock( )
        {
        }
    };

    struct SingleThreadedPolicy
    {
        using mutex_t = NullMutex;
        using lock = std::lock_guard<NullMutex>;
    };

    struct MultiThreadedPolicy
    {
        using mutex_t = std::mutex;
        using lock = std::lock_guard<std::mutex>;
    };
} // namespace vrock::log