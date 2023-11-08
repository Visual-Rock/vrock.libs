#pragma once

#include <cstdint>

namespace vrock::ui
{
    class Color
    {
    public:
        Color( );
        /**
         * @param r red color channel from 0.0 to 1.0
         * @param g green color channel from 0.0 to 1.0
         * @param b blue color channel from 0.0 to 1.0
         * */
        Color( float r, float g, float b, float a = 1.0f );

        Color( int r, int g, int b, int a = 255 );

        float r, g, b, a;
    };
} // namespace vrock::ui