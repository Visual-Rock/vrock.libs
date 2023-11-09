#pragma once

#include "RenderBackend.hpp"

struct GLFWwindow;

namespace vrock::ui
{
    class OpenGLBackend : public RenderBackend
    {
    public:
        OpenGLBackend( ) = default;

        auto init( ) -> bool final;
        auto open_window( WindowOptions options ) -> bool final;

        auto frame_begin( ) -> void override;
        auto frame_end( ) -> void override;

        auto draw_triangle( Point p1, Point p2, Point p3, Color color ) const -> void override;
        auto draw_rectangle( Point lower_left, Point upper_right, Color color ) const -> void override;
        auto draw_rectangle( Point position, int width, int height, Color color ) const -> void override;
        auto draw_circle( Point position, float radius, Color color, int resolution, float deg, float rotation ) const
            -> void override;
        auto draw_ellipse( Point position, float x, float y, Color color, int resolution, float deg,
                           float rotation ) const -> void override;

    public:
        // private:
        GLFWwindow *window = nullptr;
    };
} // namespace vrock::ui