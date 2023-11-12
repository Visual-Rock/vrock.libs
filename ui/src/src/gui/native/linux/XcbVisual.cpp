#include "vrock/ui/gui/native/linux/XcbVisual.hpp"

#include <xcb/xcb_image.h>

#include <X11/Xlib.h>

namespace vrock::ui
{
    xcb_visualtype_t *determine_proper_visualtype( xcb_screen_t *screen )
    {
        for ( auto depth_it = xcb_screen_allowed_depths_iterator( screen ); depth_it.rem; xcb_depth_next( &depth_it ) )
        {
            auto depth = depth_it.data;

            if ( depth->depth != screen->root_depth )
                continue;

            for ( auto visualtype_it = xcb_depth_visuals_iterator( depth ); visualtype_it.rem;
                  xcb_visualtype_next( &visualtype_it ) )
            {
                auto visualtype = visualtype_it.data;
                if ( visualtype->bits_per_rgb_value >= 8 )
                    return visualtype;
            }
        }
        return nullptr;
    }

    struct xcb_image_deleter
    {
        void operator( )( xcb_image_t *image ) const
        {
            xcb_image_destroy( image );
        }
    };
    using xcb_image_ptr = std::unique_ptr<xcb_image_t, xcb_image_deleter>;

    XcbVisual::XcbVisual( xcb_connection_t *connection, xcb_screen_t *screen )
        : connection( connection ), screen( screen ), visualtype{ determine_proper_visualtype( screen ) }
    {
    }

    void XcbVisual::create_surface( std::uintptr_t window )
    {
        this->window = static_cast<xcb_window_t>( window );
        gcontext = xcb_generate_id( connection );
        xcb_create_gc( connection, gcontext, this->window, 0, nullptr );
    }

    void XcbVisual::make_current( ) const
    {
    }

    void XcbVisual::swap_buffers( const Point &size ) const
    {
        xcb_image_ptr image( xcb_image_create( size.width, size.height, XCB_IMAGE_FORMAT_Z_PIXMAP, 32, 24, 32, 32,
                                               XCB_IMAGE_ORDER_LSB_FIRST, XCB_IMAGE_ORDER_LSB_FIRST, nullptr, 0,
                                               reinterpret_cast<std::uint8_t *>( pixel_data.data( ) ) ) );
        xcb_image_put( connection, window, gcontext, image.get( ), 0, 0, 0 );
    }

    auto XcbVisual::visual_id( ) const -> xcb_visualid_t
    {
        return visualtype->visual_id;
    }
} // namespace vrock::ui