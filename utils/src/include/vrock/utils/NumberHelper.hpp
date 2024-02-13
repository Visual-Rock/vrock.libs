#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace vrock::utils
{
    template <typename T>
        requires std::is_integral_v<T>
    auto to_hex( T value ) -> std::string
    {
        std::stringstream stream;
        stream << std::hex << std::setfill( '0' ) << std::setw( sizeof( T ) * 2 ) << value;
        return stream.str( );
    }
} // namespace vrock::utils