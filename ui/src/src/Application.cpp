#include "vrock/ui/Application.hpp"

#include <GLFW/glfw3.h>

namespace vrock::ui
{
    Application::Application( const ApplicationSettings settings ) : settings( settings )
    {
    }

    auto Application::run( ) -> int
    {
        if ( !glfwInit( ) )
            throw std::runtime_error( "GLFW init failed! Failed to init the GLFW library" );

        window = std::unique_ptr<Window>( new Window( settings.window.size, settings.window.title ) );

        while ( !window->should_close( ) )
        {
            glfwPollEvents( );
            window->draw( );
        }

        glfwTerminate( );
        return 0;
    }
} // namespace vrock::ui