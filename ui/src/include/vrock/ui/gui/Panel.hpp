#pragma once

#include "UIElement.hpp"

#include "../Color.hpp"
#include "../Point.hpp"

namespace vrock::ui
{
    class Panel : public UIElement
    {
    public:
        Panel( ) = default;

        auto render( const RenderBackend &renderer ) -> void override;

        auto set_corner_radius( int radius ) -> void;
        auto set_border_width( int width ) -> void;

        Point position{ 0, 0 }, size{ 0, 0 };

        Color background;

        bool round_upper_right : 1 = true;
        bool round_lower_right : 1 = true;
        bool round_upper_left : 1 = true;
        bool round_lower_left : 1 = true;

        Point upper_left_radius = { 0, 0 };
        Point lower_left_radius = { 0, 0 };
        Point upper_right_radius = { 0, 0 };
        Point lower_right_radius = { 0, 0 };

        int border_thikness_left = 0;
        int border_thikness_up = 0;
        int border_thikness_right = 0;
        int border_thikness_bottom = 0;

        Color border_color;
    };
} // namespace vrock::ui