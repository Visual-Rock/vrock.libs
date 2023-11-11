#pragma once

#include "../RasterVisual.hpp"

#include <xcb/xcb.h>

namespace vrock::ui
{
    class XcbVisual : public RasterVisual
    {
    public:
        XcbVisual( xcb_connection_t *connection, xcb_screen_t *screen );
        ~XcbVisual( ) override;

    private:
        xcb_connection_t *connection;
        xcb_screen_t *screen;
        xcb_visualtype_t *visualtype;

        xcb_window_t window;
        xcb_gcontext_t gcontext;
    };
} // namespace vrock::ui