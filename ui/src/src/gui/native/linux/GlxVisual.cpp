#include "vrock/ui/gui/native/linux/GlxVisual.hpp"

#include <cstring>
#include <iostream>

namespace vrock::ui
{
    NativeVisual::gl_function_t glx_get( void *, const char name[] )
    {
        if ( 0 == std::strncmp( name, "egl", 3 ) )
        {
            return nullptr;
        }
        return glXGetProcAddress( reinterpret_cast<const GLubyte *>( name ) );
    }

    void GlxVisual::xfree_deleter::operator( )( void *x_data )
    {
        XFree( x_data );
    }

    GlxVisual::GlxVisual( Display *display )
        : display{ display }, info{ create_xvisualinfo( ) },
          context{ glXCreateContext( display, info.get( ), nullptr, true ) }, drawable{ 0 }
    {
    }

    GlxVisual::~GlxVisual( ) = default;

    auto GlxVisual::create_surface( std::uintptr_t window ) -> void
    {
        drawable = static_cast<Window>( window );
    }

    auto GlxVisual::make_current( ) const -> void
    {
        if ( !glXQueryExtension( display, 0, 0 ) )
            std::cout << "X Server doesn't support GLX extension" << std::endl;
        if ( !glXMakeCurrent( display, drawable, context ) )
            std::cout << "glXMakeCurrent failed" << std::endl;
    }

    auto GlxVisual::swap_buffers( const Point &size ) const -> void
    {
        glXSwapBuffers( display, drawable );
    }
    auto GlxVisual::get_xvisualinfo( ) -> XVisualInfo *
    {
        return info.get( );
    }

    NativeVisual::gl_get_function_t GlxVisual::get_gl_function( ) const
    {
        return NativeVisual::get_gl_function( );
    }

    auto GlxVisual::create_xvisualinfo( ) const -> x_visual_info_t
    {
        static int attributes[]{ GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        const int default_screen = DefaultScreen( display );
        return x_visual_info_t( glXChooseVisual( display, default_screen, attributes ) );
    }
} // namespace vrock::ui