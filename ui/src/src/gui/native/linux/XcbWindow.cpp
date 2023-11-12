#include "vrock/ui/gui/native/linux/XcbWindow.hpp"

#include "vrock/ui/gui/native/linux/XcbVisual.hpp"

namespace vrock::ui
{
    XcbWindowData::XcbWindowData( ) : XcbWindowData( xcb_connect( nullptr, 0 ) )
    {
    }

    XcbWindowData::~XcbWindowData( )
    {
    }

    XcbWindowData::XcbWindowData( xcb_connection_t *connection ) : preferred_screen_index{ }, connection( connection )
    {
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator( xcb_get_setup( connection ) );
        do
        {
            if ( preferred_screen_index == 0 )
                preferred_screen = iter.data;
            --preferred_screen_index;
            xcb_screen_next( &iter );
        } while ( iter.rem != 0 );
    }

    XcbWindow::XcbWindow( ) : NativeWindow( std::make_unique<XcbVisual>( connection, preferred_screen ) )
    {
    }

    void XcbWindow::create( const vrock::ui::Point &position, const vrock::ui::Point &size, bool show_title_bar )
    {
        const auto xcb_native_visual = dynamic_cast<XcbVisual *>( native_visual.get( ) );
        if ( xcb_native_visual )
        {
            window = xcb_generate_id( connection );
            static const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
            static const uint32_t values[] = { preferred_screen->white_pixel,
                                               XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                                   XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                                                   XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_ENTER_WINDOW |
                                                   XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_KEY_PRESS |
                                                   XCB_EVENT_MASK_KEY_RELEASE };

            xcb_create_window( connection, XCB_COPY_FROM_PARENT, window, preferred_screen->root, position.x, position.y,
                               size.width, size.height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                               xcb_native_visual->visual_id( ), mask, values );
            native_visual->create_surface( window );
        }
    }

    void XcbWindow::show( )
    {
        xcb_map_window( connection, window );
        xcb_flush( connection );
    }

    void XcbWindow::hide( )
    {
        xcb_unmap_window( connection, window );
        xcb_flush( connection );
    }

    void XcbWindow::close( )
    {
        xcb_unmap_window( connection, window );
        xcb_destroy_window( connection, window );
        xcb_flush( connection );
    }

    std::string XcbWindow::get_title( )
    {
        auto cookie = xcb_get_property( connection, 0, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 0, 0 );
        auto reply = xcb_get_property_reply( connection, cookie, nullptr );

        if ( reply )
        {
            auto len = xcb_get_property_value_length( reply );
            if ( len > 0 )
            {
                auto result = static_cast<char *>( xcb_get_property_value( reply ) );
                free( reply );
                return std::string( result, len );
            }
        }
        free( reply );
        return "";
    }

    void XcbWindow::set_title( const std::string &title )
    {
        xcb_change_property( connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                             title.size( ), title.c_str( ) );
        xcb_flush( connection );
    }
} // namespace vrock::ui