#include "vrock/ui/RenderBackends/OpenGLBackend.hpp"

#include <GLFW/glfw3.h>

namespace vrock::ui
{
    bool OpenGLBackend::init( )
    {
        if ( !glfwInit( ) )
            return false;
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        return true;
    }

    bool OpenGLBackend::open_window( WindowOptions options )
    {
        options = std::move( options );
        window = glfwCreateWindow( options.width, options.height, options.window_title.c_str( ), nullptr, nullptr );
        if ( window == nullptr )
        {
            glfwTerminate( );
            return false;
        }
        glfwSetInputMode( window, GLFW_STICKY_KEYS, GLFW_TRUE );
        return true;
    }
} // namespace vrock::ui