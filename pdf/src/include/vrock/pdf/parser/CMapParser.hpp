#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace vrock::pdf
{
    class UnicodeMap
    {
    public:
        std::unordered_map<std::uint32_t, std::string> map{ };
    };

    class ByteMap
    {
    public:
        ByteMap( ) = default;

        std::unordered_map<std::uint32_t, std::uint32_t> map;
    };

    extern auto parse_unicode_map( const std::string &map ) -> std::shared_ptr<UnicodeMap>;
} // namespace vrock::pdf