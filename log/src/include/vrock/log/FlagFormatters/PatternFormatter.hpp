#pragma once

#include <vector>

#include "FlagFormatter.hpp"

namespace vrock::log
{
    using formatter_collection_t = std::vector<std::unique_ptr<FlagFormatter>>;

    auto set_global_pattern( std::string_view pattern ) -> void;
    auto get_global_pattern( ) -> std::string_view;

    auto compile_pattern( std::string_view pattern, bool use_color, bool is_file = false ) -> formatter_collection_t;
    auto compile_file_pattern( std::string_view pattern ) -> formatter_collection_t;
} // namespace vrock::log