#include "vrock/ui/gui/native/linux/XlibWindow.hpp"

#include <memory>

#include "vrock/ui/gui/native/linux/XcbVisual.hpp"

#include <X11/Xlib-xcb.h>
#include <X11/Xlibint.h>

namespace vrock::ui
{
    std::unique_ptr<NativeVisual> create_native_visuals( Display *display, const WindowFlags &flags )
    {
        if ( false && has_flag( flags, WindowFlags::OpenGL ) )
        {
        }
        else
        {
            xcb_connection_t *connection = XGetXCBConnection( display );
            xcb_screen_t *screen = ( xcb_setup_roots_iterator( xcb_get_setup( connection ) ) ).data;
            return std::make_unique<XcbVisual>( connection, screen );
        }
    }

    XlibWindow::XlibWindow( const WindowFlags &flags )
        : XlibWindowData( XOpenDisplay( nullptr ) ),
          XcbWindow( create_native_visuals( display.get( ), flags ), XGetXCBConnection( display.get( ) ) )
    {
    }

    //    void XlibWindow::create( const Point &position, const Point &size, bool show_title_bar )
    //    {
    //        Window root = DefaultRootWindow( display.get( ) );
    //
    //        XVisualInfo *xvisualinfo = get_xvisualinfo( );
    //
    //        XSetWindowAttributes window_attributes;
    //        window_attributes.colormap = XCreateColormap( display.get( ), root, xvisualinfo->visual, AllocNone );
    //        window_attributes.event_mask = ExposureMask | StructureNotifyMask | PointerMotionMask | ButtonPressMask |
    //                                       ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | KeyPressMask |
    //                                       KeyReleaseMask;
    //
    //        window = static_cast<xcb_window_t>( XCreateWindow(
    //            display.get( ), root, static_cast<int>( initial_position.x ), static_cast<int>( initial_position.y ),
    //            static_cast<unsigned int>( initial_size.width ), static_cast<unsigned int>( initial_size.width ),
    //            0, // border width
    //            xvisualinfo->depth, InputOutput, xvisualinfo->visual, CWColormap | CWEventMask, &window_attributes )
    //            );
    //
    //        native_visual->create_surface( window );
    //    }

    XlibWindow::~XlibWindow( ) = default;
} // namespace vrock::ui