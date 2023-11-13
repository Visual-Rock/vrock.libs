#include "vrock/ui/gui/native/linux/XlibWindow.hpp"

#include <memory>

#include "vrock/ui/gui/native/linux/GlxVisual.hpp"
#include "vrock/ui/gui/native/linux/XcbVisual.hpp"

#include <X11/Xlib-xcb.h>
#include <X11/Xlibint.h>

namespace vrock::ui
{
    auto make_native_window( const Point &position, const Point &size, const WindowFlags &flags )
        -> std::unique_ptr<NativeWindow>
    {
        std::unique_ptr<NativeWindow> window;
        if ( has_flag( flags, WindowFlags::OpenGL ) )
            window = std::make_unique<XlibWindow>( flags );
        else
            window = std::make_unique<XcbWindow>( );
        window->create( position, size, true );
        return window;
    }

    std::unique_ptr<NativeVisual> create_native_visuals( Display *display, const WindowFlags &flags )
    {
        if ( has_flag( flags, WindowFlags::OpenGL ) )
        {
            return std::make_unique<GlxVisual>( display );
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

    XVisualInfo *XlibWindow::get_xvisualinfo( ) const
    {
        auto glx_native_visual = dynamic_cast<GlxVisual *>( native_visual.get( ) );
        if ( glx_native_visual )
            return glx_native_visual->get_xvisualinfo( );
        else
        {
            return nullptr;
        }
    }

    void XlibWindow::create( const Point &position, const Point &size, bool show_title_bar )
    {
        Window root = DefaultRootWindow( display.get( ) );

        XVisualInfo *xvisualinfo = get_xvisualinfo( );

        XSetWindowAttributes window_attributes;
        window_attributes.colormap = XCreateColormap( display.get( ), root, xvisualinfo->visual, AllocNone );
        window_attributes.background_pixmap = None;
        window_attributes.background_pixel = 0;
        window_attributes.event_mask = ExposureMask | StructureNotifyMask | PointerMotionMask | ButtonPressMask |
                                       ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | KeyPressMask |
                                       KeyReleaseMask;

        unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
        window = static_cast<xcb_window_t>(
            XCreateWindow( display.get( ), root, position.x, position.y, static_cast<unsigned int>( size.width ),
                           static_cast<unsigned int>( size.height ),
                           0, // border width
                           xvisualinfo->depth, InputOutput, xvisualinfo->visual, mask, &window_attributes ) );

        native_visual->create_surface( window );
    }

    XlibWindow::~XlibWindow( ) = default;
} // namespace vrock::ui