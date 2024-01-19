#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace vrock::security
{
    using byte_span_t = std::span<std::uint8_t>;
    using string_view_t = std::string_view;
    using return_t = std::vector<std::uint8_t>;
} // vrock::security