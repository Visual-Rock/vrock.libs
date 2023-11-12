#pragma once

#include "XcbWindow.hpp"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>

#include "../WindowFlags.hpp"

namespace vrock::ui
{
    struct XlibWindowData
    {
        XlibWindowData( Display *display ) : display{ display }
        {
        }

        struct display_deleter
        {
            void operator( )( Display *display ) const
            {
                XCloseDisplay( display );
            }
        };
        std::unique_ptr<Display, display_deleter> display;
    };

    class XlibWindow : public XlibWindowData, public XcbWindow
    {
    public:
        explicit XlibWindow( const WindowFlags &flags );
        ~XlibWindow( ) override;
        //        auto create( const Point &position, const Point &size, bool show_title_bar ) -> void override;
    };
} // namespace vrock::ui