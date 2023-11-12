#include "vrock/ui/gui/native/NativeWindow.hpp"

namespace vrock::ui
{
    NativeWindow::NativeWindow( std::unique_ptr<NativeVisual> native_visual )
        : native_visual( std::move( native_visual ) )
    {
    }

    NativeWindow::~NativeWindow( ) = default;

    void NativeWindow::make_current( ) const
    {
        native_visual->make_current( );
    }

    void NativeWindow::swap_buffers( const Point &size )
    {
        native_visual->swap_buffers( size );
    }

    NativeVisual &NativeWindow::get_native_visual( )
    {
        return *native_visual;
    }

    const NativeVisual &NativeWindow::get_native_visual( ) const
    {
        return *native_visual;
    }
} // namespace vrock::ui