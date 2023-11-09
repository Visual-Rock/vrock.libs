#include "vrock/ui/gui/Panel.hpp"

namespace vrock::ui
{
    void Panel::render( const RenderBackend &renderer )
    {
        // |----|-----------------------------|----|
        // |  0 | r          1                | 2  |
        // |----|-----------------------------|----|
        // |  r |                             |    |
        // |    |            3                |    |
        // |  r |                             |    |
        // |----|-----------------------------|----|
        // |    | r                           |    |
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
                renderer.draw_rectangle( position, std::max( upper_left_radius.x, lower_left_radius.x ),
                                         std::max( upper_left_radius.y, upper_right_radius.y ), background );
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
                renderer.draw_rectangle( position, std::max( upper_left_radius.x, lower_left_radius.x ),
                                         std::max( upper_left_radius.y, upper_right_radius.y ), background );
            }
        }

        {
            // Segment 3
            auto pos = position;
            pos.y += upper_left_radius.y;
            renderer.draw_rectangle( pos, std::max( upper_left_radius.x, lower_left_radius.x ),
                                     size.height - upper_left_radius.height - lower_left_radius.height, background );
        }

        // {
        //     // Segment 0
        //     if ( round_upper_left )
        //     {
        //         Point pos = position;
        //         pos.x += corner_radius;
        //         pos.y += corner_radius;
        //         renderer.draw_circle( pos, corner_radius, background, std::max( 5, corner_radius ), 90, 180 );
        //     }
        //     else
        //     {
        //         renderer.draw_rectangle( position, corner_radius, corner_radius, background );
        //     }
        // }
        //
        // {
        //     // Segment 1
        //     Point pos = position;
        //     pos.x += corner_radius;
        //     Point s = size;
        //     s.x -= corner_radius * 2;
        //     s.y = corner_radius;
        //     renderer.draw_rectangle( pos, s.width, s.height, background );
        // }
        //
        // {
        //     // Segment 2
        //     if ( round_upper_right )
        //     {
        //         Point pos = position;
        //         pos.x += size.width - corner_radius;
        //         pos.y += corner_radius;
        //         renderer.draw_circle( pos, corner_radius, background, std::max( 5, corner_radius ), 90, 270 );
        //     }
        //     else
        //     {
        //         Point pos = position;
        //         pos.x += size.width - corner_radius;
        //         renderer.draw_rectangle( pos, corner_radius, corner_radius, background );
        //     }
        // }
        //
        // {
        //     // Segment 3
        //     Point pos = position;
        //     pos.y += corner_radius;
        //     renderer.draw_rectangle( pos, size.width, size.height - 2 * corner_radius, background );
        // }
        //
        // {
        //     // Segment 4
        //     Point pos = position;
        //     pos.x += corner_radius;
        //     pos.y += size.height - corner_radius;
        //     renderer.draw_circle( pos, corner_radius, background, std::max( 5, corner_radius ), 90, 90 );
        // }
        //
        // {
        //     // Segment 5
        //     Point pos = position;
        //     pos.x += corner_radius;
        //     pos.y += size.height - corner_radius;
        //     renderer.draw_rectangle( pos, size.width - corner_radius * 2, corner_radius, background );
        // }
        //
        // {
        //     // Segment 5
        //     Point pos = position;
        //     pos.x += size.width - corner_radius;
        //     pos.y += size.height - corner_radius;
        //     renderer.draw_circle( pos, corner_radius, background, std::max( 5, corner_radius ), 90 );
        // }
    }

    auto Panel::set_corner_radius( int radius ) -> void
    {
        lower_left_radius = { radius, radius };
        upper_left_radius = { radius, radius };
        lower_right_radius = { radius, radius };
        upper_right_radius = { radius, radius };
    }
} // namespace vrock::ui