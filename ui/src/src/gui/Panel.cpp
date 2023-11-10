#include "vrock/ui/gui/Panel.hpp"

namespace vrock::ui
{
    void Panel::render( const RenderBackend &renderer )
    {
        if ( border_thikness_left != 0 || border_thikness_up != 0 || border_thikness_right != 0 ||
             border_thikness_bottom != 0 )
        {
            auto border = Panel{ };
            border.position = position;
            border.position.x -= border_thikness_left;
            border.position.y -= border_thikness_up;

            border.round_upper_left = round_upper_left;
            border.upper_left_radius = upper_left_radius;
            border.upper_left_radius.width += border_thikness_left;
            border.upper_left_radius.height += border_thikness_up;

            border.round_upper_right = round_upper_right;
            border.upper_right_radius = upper_right_radius;
            border.upper_right_radius.width += border_thikness_right;
            border.upper_right_radius.height += border_thikness_up;

            border.round_lower_left = round_lower_left;
            border.lower_left_radius = lower_left_radius;
            border.lower_left_radius.width += border_thikness_left;
            border.lower_left_radius.height += border_thikness_bottom;

            border.round_lower_right = round_lower_right;
            border.lower_right_radius = lower_right_radius;
            border.lower_right_radius.width += border_thikness_right;
            border.lower_right_radius.height += border_thikness_bottom;

            border.size = size;
            border.size.width += border_thikness_left + border_thikness_right;
            border.size.height += border_thikness_bottom + border_thikness_up;
            border.background = border_color;

            border.render( renderer );
        }

        // |----|-----------------------------|----|
        // |  0 |            1                | 2  |
        // |----|-----------------------------|----|
        // |    |                             |    |
        // |  3 |            4                | 5  |
        // |    |                             |    |
        // |----|-----------------------------|----|
        // |  6 |            7                | 8  |
        // |----|-----------------------------|----|

        {
            // Segment 0
            if ( round_upper_left )
            {
                auto pos = position;
                pos.x += upper_left_radius.x;
                pos.y += upper_left_radius.y;
                renderer.draw_ellipse( pos, upper_left_radius.x, upper_left_radius.y, background,
                                       std::max( 5, std::max( upper_left_radius.x, upper_left_radius.y ) ), 90, 180 );
            }
            else
            {
                renderer.draw_rectangle( position, upper_left_radius.width, upper_left_radius.height, background );
            }
        }

        {
            // Segment 1
            auto pos = position;
            pos.x += upper_left_radius.x;
            renderer.draw_rectangle( pos, size.width - upper_left_radius.x - upper_right_radius.x,
                                     std::max( upper_left_radius.y, upper_right_radius.y ), background );
        }

        {
            // Segment 2
            if ( round_upper_right )
            {
                auto pos = position;
                pos.x += size.width - upper_right_radius.x;
                pos.y += upper_right_radius.y;
                renderer.draw_ellipse( pos, upper_right_radius.x, upper_right_radius.y, background,
                                       std::max( 5, std::max( upper_right_radius.x, upper_right_radius.y ) ), 90, 270 );
            }
            else
            {
                auto pos = position;
                pos.x += size.width - upper_right_radius.x;
                renderer.draw_rectangle( pos, upper_right_radius.width, upper_right_radius.height, background );
            }
        }

        {
            // Segment 3
            auto pos = position;
            pos.y += upper_left_radius.y;
            renderer.draw_rectangle( pos, std::max( upper_left_radius.x, lower_left_radius.x ),
                                     size.height - upper_left_radius.height - lower_left_radius.height, background );
        }

        {
            // Segment 4
            auto pos = position;
            pos.x += std::max( upper_left_radius.x, lower_left_radius.x );
            pos.y += std::max( upper_left_radius.y, upper_right_radius.y );
            renderer.draw_rectangle( pos,
                                     size.width - std::max( upper_right_radius.x, lower_right_radius.x ) -
                                         std::max( upper_left_radius.x, lower_left_radius.x ),
                                     size.height - std::max( upper_right_radius.y, upper_left_radius.y ) -
                                         std::max( lower_right_radius.y, lower_left_radius.y ),
                                     background );
        }

        {
            // Segment 5
            auto pos = position;
            pos.y += upper_right_radius.y;
            pos.x += size.width - std::max( upper_right_radius.x, lower_right_radius.x );
            renderer.draw_rectangle( pos, std::max( upper_right_radius.x, lower_right_radius.x ),
                                     size.height - upper_right_radius.height - lower_right_radius.height, background );
        }

        {
            // Segment 6
            if ( round_lower_left )
            {
                auto pos = position;
                pos.x += lower_left_radius.x;
                pos.y += size.height - lower_left_radius.y;
                renderer.draw_ellipse( pos, lower_left_radius.x, lower_left_radius.y, background,
                                       std::max( 5, std::max( lower_left_radius.x, lower_left_radius.y ) ), 90, 90 );
            }
            else
            {
                auto pos = position;
                pos.y += size.height - lower_left_radius.y;
                renderer.draw_rectangle( pos, lower_left_radius.width, lower_left_radius.height, background );
            }
        }

        {
            // Segment 7
            auto pos = position;
            pos.x += lower_left_radius.x;
            pos.y += size.height - std::max( lower_left_radius.y, lower_right_radius.y );
            renderer.draw_rectangle( pos, size.width - lower_left_radius.x - lower_right_radius.x,
                                     std::max( lower_left_radius.y, lower_right_radius.y ), background );
        }

        {
            // Segment 8
            if ( round_lower_right )
            {
                auto pos = position;
                pos.x += size.width - lower_right_radius.x;
                pos.y += size.height - lower_right_radius.y;
                renderer.draw_ellipse( pos, lower_right_radius.x, lower_right_radius.y, background,
                                       std::max( 5, std::max( lower_right_radius.x, lower_right_radius.y ) ), 90 );
            }
            else
            {
                auto pos = position;
                pos.x += size.width - lower_right_radius.x;
                pos.y += size.height - lower_right_radius.y;
                renderer.draw_rectangle( pos, lower_right_radius.width, lower_right_radius.height, background );
            }
        }
    }

    auto Panel::set_corner_radius( int radius ) -> void
    {
        lower_left_radius = { radius, radius };
        upper_left_radius = { radius, radius };
        lower_right_radius = { radius, radius };
        upper_right_radius = { radius, radius };
    }

    auto Panel::set_border_width( int width ) -> void
    {
        border_thikness_left = width;
        border_thikness_up = width;
        border_thikness_right = width;
        border_thikness_bottom = width;
    }
} // namespace vrock::ui