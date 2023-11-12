#include "vrock/ui/gui/Window.hpp"

namespace vrock::ui
{
    Window::~Window( )
    {
        if ( native_window )
            native_window->close( );
    }
} // namespace vrock::ui