#pragma once

#include "../NativeWindow.hpp"

#include <xcb/xcb.h>

namespace vrock::ui
{
    struct XcbWindowData
    {
    protected:
        XcbWindowData( );
        virtual ~XcbWindowData( );
        XcbWindowData( xcb_connection_t *connection );

        int preferred_screen_index;
        xcb_connection_t *connection;
        xcb_screen_t *preferred_screen;
    };

    class XcbWindow : public XcbWindowData, public NativeWindow
    {
    public:
        XcbWindow( );
        ~XcbWindow( ) = default;

        auto create( const Point &position, const Point &size, bool show_title_bar ) -> void override;
        auto show( ) -> void override;
        auto hide( ) -> void override;
        auto close( ) -> void override;
        auto get_title( ) -> std::string override;
        auto set_title( const std::string &title ) -> void override;

    protected:
        XcbWindow( std::unique_ptr<NativeVisual> &&nv, xcb_connection_t *connection )
            : XcbWindowData( connection ), NativeWindow( std::move( nv ) )
        {
        }

        xcb_window_t window;
    };
} // namespace vrock::ui