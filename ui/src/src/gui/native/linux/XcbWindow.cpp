#include "vrock/ui/gui/native/linux/XcbWindow.hpp"

namespace vrock::ui::native
{
    void Xcb::create( const vrock::ui::Point &position, const vrock::ui::Point &size, bool show_title_bar )
    {
    }

    void Xcb::show( )
    {
        xcb_map_window( connection, window );
        xcb_flush( connection );
    }

    void Xcb::hide( )
    {
        xcb_unmap_window( connection, window );
        xcb_flush( connection );
    }

    void Xcb::close( )
    {
        xcb_unmap_window( connection, window );
        xcb_destroy_window( connection, window );
        xcb_flush( connection );
    }

    std::string Xcb::get_title( )
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

    void Xcb::set_title( const std::string &title )
    {
        xcb_change_property( connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                             title.size( ), title.c_str( ) );
        xcb_flush( connection );
    }
} // namespace vrock::ui::native