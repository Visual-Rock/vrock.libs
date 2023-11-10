#pragma once

#include "RenderBackend.hpp"

namespace vrock::ui
{
    class X11RenderBackend : public RenderBackend
    {
    public:
        auto init( ) -> bool override;
        auto open_window( WindowOptions options ) -> bool override;
    };
} // namespace vrock::ui
