#include "vrock/ui/gui/Window.hpp"

#include "../../../../utils/src/include/vrock/utils/Timer.hpp"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

namespace vrock::ui
{
    std::vector<std::pair<GLFWwindow *, Window *>> windows = { };

    void frame_buffer_size_callback( GLFWwindow *fwindow, int w, int h )
    {
        for ( auto &[ k, v ] : windows )
        {
            if ( fwindow == k )
            {
                v->size = { w, h };
                v->update_surface( );
                v->pixels = std::vector<std::uint8_t>( w * h * 4 );
                break;
            }
        }
    }

    Window::Window( Point initial_size, const std::string &title ) : size( initial_size )
    {
        window = glfwCreateWindow( size.width, size.height, title.c_str( ), nullptr, nullptr );
        glfwGetFramebufferSize( window, &( size.width ), &( size.height ) );
        glfwSetWindowSizeCallback( window, frame_buffer_size_callback );
        glfwSetFramebufferSizeCallback( window, frame_buffer_size_callback );
        pixels = std::vector<std::uint8_t>( size.width * size.height * 4 );
        windows.emplace_back( window, this );
        update_surface( );
    }

    Window::~Window( )
    {
    }

    auto Window::show( ) -> void
    {
    }

    auto Window::hide( ) -> void
    {
    }

    auto Window::close( ) -> void
    {
        glfwDestroyWindow( window );
    }
    auto Window::should_close( ) -> bool
    {
        return glfwWindowShouldClose( window );
    }

    SkPaint getPaint( SkColor s )
    {
        SkPaint paint;
        paint.setColor( s );
        return paint;
    }

    auto Window::draw( ) -> void
    {
        glfwMakeContextCurrent( window );
        // update ui positioning + rendering

        {
            utils::ScopedTimer<> t( []( auto x ) { std::cout << x << "ms" << std::endl; } );
            for ( int i = 0; i < 10000; ++i )
            {
                SkPaint paint( SkColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
                SkRect rect( 100, 50, 600, 50 );
                surface->getCanvas( )->drawRect( SkRect::MakeXYWH( 0, 0, 250, 250 ), getPaint( SK_ColorRED ) );
            }
        }

        // render to window
        {
            surface->readPixels( info, pixels.data( ), size.width * sizeof( uint8_t ) * 4, 0, 0 );
            glDrawPixels( size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data( ) );
            glfwSwapBuffers( window );
        }
    }

    auto Window::update_surface( ) -> void
    {
        info = SkImageInfo::Make( size.width, size.height, SkColorType::kRGBA_8888_SkColorType,
                                  SkAlphaType::kPremul_SkAlphaType );
        surface = SkSurface::MakeRaster( info );
        canvas = surface->getCanvas( );
        canvas->translate( 0.0f, size.height );
        canvas->scale( 1.0f, -1.0f );
    }

} // namespace vrock::ui