#include "vrock/ui.hpp"

#include "vrock/ui/Color.hpp"

#include "vrock/ui/gui/Window.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <thread>

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlibint.h>

vrock::ui::Color dark0 = vrock::ui::Color( 0x2e, 0x34, 0x40 );
vrock::ui::Color dark1 = vrock::ui::Color( 0x3b, 0x42, 0x52 );
vrock::ui::Color dark2 = vrock::ui::Color( 0x43, 0x4c, 0x5e );
vrock::ui::Color dark3 = vrock::ui::Color( 0x4c, 0x56, 0x6a );

vrock::ui::Color light0 = vrock::ui::Color( 0xcc, 0xcc, 0xcc );
vrock::ui::Color light1 = vrock::ui::Color( 0xe5, 0xe9, 0xf0 );
vrock::ui::Color light2 = vrock::ui::Color( 0xec, 0xef, 0xf4 );

auto create_xvisualinfo( auto display ) -> XVisualInfo *
{
    static int attributes[]{ GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    const int default_screen = DefaultScreen( display );
    return glXChooseVisual( display, default_screen, attributes );
}

int main( )
{
    // auto renderer = vrock::ui::X11RenderBackend( );
    // if ( !renderer.open_window( { 800, 600, "Test", dark0 } ) )
    //     return 1;

    std::cout << "Hello World!" << std::endl;

    auto window = std::make_unique<vrock::ui::Window>( vrock::ui::Point{ 200, 200 }, vrock::ui::Point{ 800, 600 },
                                                       vrock::ui::WindowFlags::_None );
    // window->create( { 200, 200 }, { 800, 600 }, true );
    bool show = true;
    while ( true )
    {
        std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
        if ( show )
            window->show( );
        else
        {
            window->hide( );
        }
        show = !show;
    }

    return 0;
}
