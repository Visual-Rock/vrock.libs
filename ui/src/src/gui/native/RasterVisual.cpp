#include "vrock/ui/gui/native/RasterVisual.hpp"

namespace vrock::ui
{
    std::vector<std::uint32_t> &RasterVisual::pixels( )
    {
        return pixel_data;
    }
    const std::vector<std::uint32_t> &RasterVisual::pixels( ) const
    {
        return pixel_data;
    }
} // namespace vrock::ui