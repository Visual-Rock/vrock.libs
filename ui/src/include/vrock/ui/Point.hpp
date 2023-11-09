#pragma once

#include <cstdint>

namespace vrock::ui
{
    struct Point
    {
        Point( ) : x( 0 ), y( 0 )
        {
        }

        Point( int _1, int _2 ) : x( _1 ), y( _2 )
        {
        }

        union {
            int width;
            int x;
        };

        union {
            int height;
            int y;
        };
    };
} // namespace vrock::ui