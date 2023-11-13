#include "vrock/ui/gui/Window.hpp"

#include "vrock/ui/gui/native/Context.hpp"

namespace vrock::ui
{
    Window::Window( Point position, Point initial_size, WindowFlags flags )
        : position( position ), size( size ), native_window( nullptr ), flags( flags )
    {
        std::unique_lock lock{ mutex };
        std::atomic_bool initialized{ false };

        std::thread t( &Window::initialize_execute_platform_loop, this, std::ref( initialized ) );
        event_thread.swap( t );
        conditional_var.wait( lock, [ & ] { return initialized.load( ); } );
        lock.unlock( );
    }

    Window::Window( WindowFlags flags )
    {
    }

    Window::~Window( )
    {
        if ( native_window )
            native_window->close( );
    }

    auto Window::show( ) -> void
    {
        native_window->show( );
    }

    auto Window::hide( ) -> void
    {
        native_window->hide( );
    }

    auto Window::close( ) -> void
    {
        native_window->close( );
    }

    auto Window::initialize_execute_platform_loop( std::atomic_bool &initialized ) -> void
    {
        native_window = make_native_window( position, size, flags );

        auto t = std::thread( &Window::create_gcontext_execute_render_loop, this, std::ref( initialized ) );
        render_thread.swap( t );

        std::unique_lock lock{ mutex };
        conditional_var.wait( lock, [ & ] { return initialized.load( ); } );
        lock.release( );

        native_window.reset( );
    }

    auto Window::create_gcontext_execute_render_loop( std::atomic_bool &initialized ) -> void
    {
        if ( has_flag( flags, WindowFlags::OpenGL ) )
        {
            show( );
            const auto &native_visual = native_window->get_native_visual( );
            native_visual.make_current( );
            context = std::make_unique<OpenGlContext>( native_visual.get_gl_function( ) );
        }

        {
            std::unique_lock lock{ mutex };
        }
        initialized = true;

        conditional_var.notify_all( );

        while ( true )
        {
        }

        context.reset( );
    }
} // namespace vrock::ui