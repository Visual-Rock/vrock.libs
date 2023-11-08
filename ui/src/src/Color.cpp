#include "vrock/ui/Color.hpp"

namespace vrock::ui
{
    Color::Color( float r, float g, float b, float a ) : r( r ), g( g ), b( b ), a( a )
    {
    }

    Color::Color( int r, int g, int b, int a ) : r( r / 255.0f ), g( g / 255.0f ), b( b / 255.0f ), a( a / 255.0f )
    {
    }
    Color::Color( ) : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f )
    {
    }
} // namespace vrock::ui
