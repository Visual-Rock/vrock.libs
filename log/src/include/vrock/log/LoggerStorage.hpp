#pragma once

#include <vrock/log/Logger.hpp>

namespace vrock::log
{
    auto make_logger( std::string_view name, const LogLevel level = get_global_log_level( ),
                      const bool multi_threaded = false, const std::string_view pattern = get_global_pattern( ) )
        -> logger_t;

    auto get_logger( const std::string_view name ) -> logger_t;

    inline bool add_standard_out_to_default = true;
} // namespace vrock::log