#pragma once

#include <memory>

#include "../Point.hpp"

#include "include/core/SkSurface.h"

typedef struct GLFWwindow GLFWwindow;

namespace vrock::ui
{
    class Application;

    struct WindowSettings
    {
        Point size;
        std::string title;
    };

    class Window
    {
        friend Application;
        friend void frame_buffer_size_callback( GLFWwindow *fwindow, int w, int h );

        Window( Point initial_size = { 800, 600 }, const std::string &title = "vrock.ui" );

    public:
        ~Window( );

        auto show( ) -> void;
        auto hide( ) -> void;
        auto close( ) -> void;

        auto should_close( ) -> bool;

        auto draw( ) -> void;

        auto update_surface( ) -> void;

        auto create_child_window( ) -> int;

        Point size;

    private:
        std::vector<std::unique_ptr<Window>> children = { };

        SkImageInfo info;
        sk_sp<SkSurface> surface;
        SkCanvas *canvas;

        std::vector<std::uint8_t> pixels;

        GLFWwindow *window;
    };
} // namespace vrock::ui