#pragma once

#include "NativeVisual.hpp"
#include "NativeWindow.hpp"

#include <vector>

namespace vrock::ui
{
    class RasterVisual : public NativeVisual
    {
    public:
        RasterVisual( ) = default;
        ~RasterVisual( ) override = default;

        std::vector<std::uint32_t> &pixels( );
        const std::vector<std::uint32_t> &pixels( ) const;

    protected:
        mutable std::vector<std::uint32_t> pixel_data;
    };
} // namespace vrock::ui