#pragma once

#include <sstream>
#include <string>
#include <thread>

#ifdef VROCKLIBS_LOG_USE_PROCESS_ID
#include <unistd.h>
#endif

namespace vrock::log
{
    class ExecutionContext
    {
        using thread_id_t = std::string;
        using process_id_t = std::size_t;

    public:
        ExecutionContext( )
        {
#ifdef VROCKLIBS_LOG_USE_THREAD_ID
            std::ostringstream ss;
            ss << std::this_thread::get_id( );
            thread_id = ss.str( );
#endif
#ifdef VROCKLIBS_LOG_USE_PROCESS_ID
            process_id = getpid( );
#endif
        }

        thread_id_t thread_id;
        process_id_t process_id = 0;
    };

    inline thread_local ExecutionContext this_execution_context;
} // namespace vrock::log