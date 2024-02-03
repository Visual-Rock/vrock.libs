#pragma once

#include <vrock/log/Logger.hpp>

namespace vrock::log
{
    /**
     * @brief creates a logger with the given name, level, multi_threaded, and pattern
     * @param name the name of the logger
     * @param level the level of the logger
     * @param multi_threaded if the logger is used in a multi-threaded environment
     * @param pattern the pattern of the logger
     * @return a shared pointer to the logger
     */
    auto make_logger( std::string_view name, const LogLevel level = get_global_log_level( ),
                      const bool multi_threaded = false, const std::string_view pattern = get_global_pattern( ) )
        -> logger_t;

    /**
     * @brief gets the logger with the given name or creates a new one if it does not exist yet.
     * @param name the name of the logger
     * @return a shared pointer to the logger
     */
    auto get_logger( const std::string_view name ) -> logger_t;
    inline bool add_standard_out_to_default = true;
} // namespace vrock::log