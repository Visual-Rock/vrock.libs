#include "vrock/ui.hpp"

#include <iostream>

int main( )
{
    auto renderer = vrock::ui::OpenGLBackend( );
    if ( !renderer.init( ) )
        std::cout << "init failed" << std::endl;

    if ( !renderer.open_window( { 600, 800, "Test" } ) )
        std::cout << "init failed" << std::endl;

    while ( true )
    {
    }

    return 0;
}