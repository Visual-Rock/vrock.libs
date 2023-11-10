#pragma once

#include <cstdint>
#include <string>

#include "vrock/ui/Color.hpp"
#include "vrock/ui/Point.hpp"

#include <cairo/cairo.h>

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

        // virtual auto frame_begin( ) -> void = 0;
        // virtual auto frame_end( ) -> void = 0;
        //
        // virtual auto draw_triangle( Point p1, Point p2, Point p3, Color color ) const -> void = 0;
        // virtual auto draw_rectangle( Point lower_left, Point upper_right, Color color ) const -> void = 0;
        // virtual auto draw_rectangle( Point position, int width, int height, Color color ) const -> void = 0;
        // virtual auto draw_circle( Point position, float radius, Color color, int resolution = 25, float degrees =
        // 360,
        //                           float rotation = 0 ) const -> void = 0;
        // virtual auto draw_ellipse( Point position, float x, float y, Color color, int resolution = 25, float deg =
        // 360,
        //                            float rotation = 0 ) const -> void = 0;

        WindowOptions options;

    protected:
        cairo_surface_t *_surface;
        cairo_t *_cairo;
    };
} // namespace vrock::ui