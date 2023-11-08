#include "vrock/ui.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <string>

void drawRoundedRect( float x, float y, float width, float height, float radius )
{
    const int num_segments = 100; // increase for better resolution
    const float PI = 3.14159f;

    glBegin( GL_POLYGON );
    // Top left corner
    for ( int ii = num_segments; ii != 75; ii-- )
    {
        float theta = 2.0f * PI * float( ii ) / float( num_segments );
        float dx = radius * cosf( theta );
        float dy = radius * sinf( theta );
        glVertex2f( x + dx, y - dy );
    }
    // Top right corner
    for ( int ii = 75; ii != 50; ii-- )
    {
        float theta = 2.0f * PI * float( ii ) / float( num_segments );
        float dx = radius * cosf( theta );
        float dy = radius * sinf( theta );
        glVertex2f( x + width + dx, y - dy );
    }
    // Bottom right corner
    for ( int ii = 50; ii != 25; ii-- )
    {
        float theta = 2.0f * PI * float( ii ) / float( num_segments );
        float dx = radius * cosf( theta );
        float dy = radius * sinf( theta );
        glVertex2f( x + width + dx, y - height - dy );
    }
    // Bottom left corner
    for ( int ii = 25; ii != 0; ii-- )
    {
        float theta = 2.0f * PI * float( ii ) / float( num_segments );
        float dx = radius * cosf( theta );
        float dy = radius * sinf( theta );
        glVertex2f( x + dx, y - height - dy );
    }
    glEnd( );
}

vrock::ui::Color dark0 = vrock::ui::Color( 0x2e, 0x34, 0x40 );
vrock::ui::Color dark1 = vrock::ui::Color( 0x3b, 0x42, 0x52 );
vrock::ui::Color dark2 = vrock::ui::Color( 0x43, 0x4c, 0x5e );
vrock::ui::Color dark3 = vrock::ui::Color( 0x4c, 0x56, 0x6a );

vrock::ui::Color light0 = vrock::ui::Color( 0xcc, 0xcc, 0xcc );
vrock::ui::Color light1 = vrock::ui::Color( 0xe5, 0xe9, 0xf0 );
vrock::ui::Color light2 = vrock::ui::Color( 0xec, 0xef, 0xf4 );

int main( )
{
    auto renderer = vrock::ui::OpenGLBackend( );
    if ( !renderer.init( ) )
        std::cout << "init failed" << std::endl;

    if ( !renderer.open_window( { 800, 600, "Test", vrock::ui::Color( 0x2e, 0x34, 0x40, 0xff ) } ) )
        std::cout << "init failed" << std::endl;
    glViewport( 0, 0, 800, 600 );

    while ( !glfwWindowShouldClose( renderer.window ) )
    {
        renderer.frame_begin( );

        glColor3f( 1.0, 0.0, 0.0 );

        renderer.draw_triangle( { 0, 0 }, { 300, 0 }, { 0, 300 }, dark1 );

        renderer.draw_rectangle( { 300, 300 }, 100, 50, dark1 );

        renderer.frame_end( );
    }

    return 0;
}
