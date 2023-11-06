#pragma once

#include "RenderBackend.hpp"

struct GLFWwindow;

namespace vrock::ui
{
    class OpenGLBackend : RenderBackend
    {
    public:
        OpenGLBackend( ) = default;

        auto init( ) -> bool final;
        auto open_window( WindowOptions options ) -> bool final;

    private:
        GLFWwindow *window = nullptr;
    };
} // namespace vrock::ui