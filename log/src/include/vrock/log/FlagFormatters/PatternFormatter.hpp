#pragma once

#include <vector>

#include "FlagFormatter.hpp"

namespace vrock::log
{
    using formatter_collection_t = std::vector<std::unique_ptr<FlagFormatter>>;

    auto compile_pattern( std::string_view pattern, bool use_color ) -> formatter_collection_t;
} // namespace vrock::log