#pragma once

#include <memory>

#include "../Point.hpp"

#include "native/Context.hpp"
#include "native/NativeWindow.hpp"

#include <atomic>
#include <condition_variable>
#include <thread>

namespace vrock::ui
{
    class Window
    {
    public:
        Window( Point position = { 0, 0 }, Point initial_size = { 800, 600 },
                WindowFlags flags = WindowFlags::AntiAlias );
        explicit Window( WindowFlags flags );
        ~Window( );

        auto show( ) -> void;
        auto hide( ) -> void;
        auto close( ) -> void;

        Point position, size;
        WindowFlags flags;

    private:
        auto initialize_execute_platform_loop( std::atomic_bool &initialized ) -> void;
        auto create_gcontext_execute_render_loop( std::atomic_bool &initialized ) -> void;

        std::thread render_thread;
        std::thread event_thread;

        std::unique_ptr<NativeWindow> native_window = nullptr;
        std::unique_ptr<GraphicsContext> context;

        std::mutex mutex;
        std::condition_variable conditional_var;
    };
} // namespace vrock::ui