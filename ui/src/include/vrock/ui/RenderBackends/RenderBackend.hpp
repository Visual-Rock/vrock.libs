#pragma once

#include <cstdint>
#include <string>

#include "vrock/ui/Color.hpp"
#include "vrock/ui/Point.hpp"

namespace vrock::ui
{
    struct WindowOptions
    {
        int width, height;
        std::string window_title;
        Color background;
    };

    class RenderBackend
    {
    public:
        RenderBackend( ) = default;

        virtual auto init( ) -> bool = 0;
        virtual auto open_window( WindowOptions options ) -> bool = 0;

        virtual auto frame_begin( ) -> void = 0;
        virtual auto frame_end( ) -> void = 0;

        virtual auto draw_triangle( Point p1, Point p2, Point p3, Color color ) -> void = 0;
        virtual auto draw_rectangle( Point lower_left, Point upper_right, Color color ) -> void = 0;
        virtual auto draw_rectangle( Point position, int width, int height, Color color ) -> void = 0;

        WindowOptions options;
    };
} // namespace vrock::ui