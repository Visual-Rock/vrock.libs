#pragma once

#include "../NativeWindow.hpp"

#include <xcb/xcb.h>

namespace vrock::ui::native
{
    class Xcb : public NativeWindow
    {
    public:
        Xcb( ) = default;
        auto create( const Point &position, const Point &size, bool show_title_bar ) -> void override;
        auto show( ) -> void override;
        auto hide( ) -> void override;
        auto close( ) -> void override;
        auto get_title( ) -> std::string override;
        auto set_title( const std::string &title ) -> void override;

    protected:
        int preferred_screen_index;
        xcb_connection_t *connection;
        xcb_screen_t *preferred_screen;
        xcb_window_t window;
    };
} // namespace vrock::ui::native