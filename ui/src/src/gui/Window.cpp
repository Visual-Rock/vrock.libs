#include "vrock/ui/gui/Window.hpp"

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include <GLFW/glfw3.h>
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
                break;
            }
        }
    }

    Window::Window( Point position, Point initial_size ) : position( position ), size( size )
    {
        if ( !glfwInit( ) )
        {
            // TODO: Handle error
        }
        window = glfwCreateWindow( size.width, size.height, "Skia Test", nullptr, nullptr );
        glfwMakeContextCurrent( window );
        glfwGetFramebufferSize( window, &( size.width ), &( size.height ) );
        glfwSetFramebufferSizeCallback( window, frame_buffer_size_callback );
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
        glfwTerminate( );
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