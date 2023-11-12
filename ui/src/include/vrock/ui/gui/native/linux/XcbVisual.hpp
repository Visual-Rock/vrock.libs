#pragma once

#include "../RasterVisual.hpp"

#include <xcb/xcb.h>

namespace vrock::ui
{
    class XcbVisual : public RasterVisual
    {
    public:
        XcbVisual( xcb_connection_t *connection, xcb_screen_t *screen );
        ~XcbVisual( ) override = default;

        auto create_surface( std::uintptr_t window ) -> void override;
        auto make_current( ) const -> void override;
        auto swap_buffers( const Point &size ) const -> void override;

        auto visual_id( ) const -> xcb_visualid_t;

    private:
        xcb_connection_t *connection;
        xcb_screen_t *screen;
        xcb_visualtype_t *visualtype = nullptr;

        xcb_window_t window;
        xcb_gcontext_t gcontext;
    };
} // namespace vrock::ui