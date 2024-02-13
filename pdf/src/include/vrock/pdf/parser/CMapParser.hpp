#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

namespace vrock::pdf
{
    class CMap
    {
    public:
        CMap( ) = default;

        std::unordered_map<std::uint32_t, std::uint32_t> map;
    };

    extern auto parse_cmap( const std::string &cmap ) -> std::shared_ptr<CMap>;
} // namespace vrock::pdf