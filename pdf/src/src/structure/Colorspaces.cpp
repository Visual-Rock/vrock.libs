#include "vrock/pdf/structure/Colorspaces.hpp"

namespace vrock::pdf
{
    auto get_num( std::uint8_t *data, std::size_t idx, std::uint8_t stride ) -> std::uint8_t
    {
        auto bit_offset = idx * stride;
        auto byte_offset = bit_offset / 8;
        auto offset = 7 - ( bit_offset % 8 );
        auto mask = ( ( (uint8_t)std::pow( 2, stride ) ) - 1 ) << offset;
        return ( data[ byte_offset ] & mask ) >> offset;
    }
} // namespace vrock::pdf