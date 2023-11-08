#include "vrock/ui/RenderBackends/OpenGLBackend.hpp"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace vrock::ui
{
    void framebuffer_size_callback( GLFWwindow *window, int width, int height )
    {
        glViewport( 0, 0, width, height );
    }

    bool OpenGLBackend::init( )
    {
        if ( !glfwInit( ) )
            return false;

        if ( !glewInit( ) )
        {
            glfwTerminate( );
            return false;
        }

        // glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        // glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
        // glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        return true;
    }

    bool OpenGLBackend::open_window( WindowOptions _options )
    {
        options = std::move( _options );
        window = glfwCreateWindow( options.width, options.height, options.window_title.c_str( ), nullptr, nullptr );
        if ( window == nullptr )
        {
            glfwTerminate( );
            return false;
        }
        glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
        glfwSetInputMode( window, GLFW_STICKY_KEYS, GLFW_TRUE );
        glfwMakeContextCurrent( window );
        return true;
    }

    void OpenGLBackend::frame_begin( )
    {
        glfwPollEvents( );
        glClearColor( options.background.r, options.background.g, options.background.b, options.background.a );
        glClear( GL_COLOR_BUFFER_BIT );
        glfwGetWindowSize( window, &options.width, &options.height );
    }

    void OpenGLBackend::frame_end( )
    {
        glfwSwapBuffers( window );
    }

    std::pair<float, float> convert_point( const WindowOptions &options, const Point &point )
    {
        return { ( (float)point.x / (float)options.width ) * 2.0f - 1.0f,
                 ( (float)point.y / (float)options.height ) * 2.0f - 1.0f };
    }

    void OpenGLBackend::draw_triangle( Point p1, Point p2, Point p3, Color color )
    {
        auto [ x1, y1 ] = convert_point( options, p1 );
        auto [ x2, y2 ] = convert_point( options, p2 );
        auto [ x3, y3 ] = convert_point( options, p3 );
        glColor4f( color.r, color.g, color.b, color.a );
        glBegin( GL_TRIANGLES );
        glVertex2f( x1, y1 );
        glVertex2f( x2, y2 );
        glVertex2f( x3, y3 );
        glEnd( );
    }

    void OpenGLBackend::draw_rectangle( Point lower_left, Point upper_right, Color color )
    {
        auto [ x1, y1 ] = convert_point( options, lower_left );
        auto [ x2, y2 ] = convert_point( options, upper_right );
        glColor4f( color.r, color.g, color.b, color.a );
        glBegin( GL_QUADS );
        glVertex2f( x1, y1 );
        glVertex2f( x2, y1 );
        glVertex2f( x2, y2 );
        glVertex2f( x1, y2 );
        glEnd( );
    }

    void OpenGLBackend::draw_rectangle( Point position, int width, int height, Color color )
    {
        draw_rectangle( position, { position.x + width, position.y + height }, color );
    }
} // namespace vrock::ui