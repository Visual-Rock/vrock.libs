#pragma once

#include <memory>

#include "../Point.hpp"

#include "include/core/SkSurface.h"

typedef struct GLFWwindow GLFWwindow;

namespace vrock::ui
{
    class Window
    {
    public:
        Window( Point position = { 0, 0 }, Point initial_size = { 800, 600 } );
        ~Window( );

        auto show( ) -> void;
        auto hide( ) -> void;
        auto close( ) -> void;

        auto update_surface( ) -> void;

        Point position, size;

    private:
        SkImageInfo info;
        sk_sp<SkSurface> surface;
        SkCanvas *canvas;

        GLFWwindow *window;
    };
} // namespace vrock::ui