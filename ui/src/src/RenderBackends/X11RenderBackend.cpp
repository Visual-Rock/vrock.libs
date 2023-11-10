#include "vrock/ui/RenderBackends/X11Renderbackend.hpp"

#include <cairo-xlib.h>

namespace vrock::ui
{
    auto X11RenderBackend::init( ) -> bool
    {
        return true;
    }
    auto X11RenderBackend::open_window( WindowOptions _options ) -> bool
    {
        options = std::move( _options );
        Display *dsp;
        Drawable da;
        int screen;

        if ( ( dsp = XOpenDisplay( NULL ) ) == NULL )
            return false;
        screen = DefaultScreen( dsp );
        da = XCreateSimpleWindow( dsp, DefaultRootWindow( dsp ), 0, 0, options.width, options.height, 0, 0, 0 );
        XSelectInput( dsp, da, ButtonPressMask | KeyPressMask );
        XMapWindow( dsp, da );

        _surface = cairo_xlib_surface_create( dsp, da, DefaultVisual( dsp, screen ), options.width, options.height );
        cairo_xlib_surface_set_size( _surface, options.width, options.height );

        return true;
    }
} // namespace vrock::ui