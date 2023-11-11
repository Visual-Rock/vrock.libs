#pragma once

#include "RenderBackend.hpp"

struct GLFWwindow;

namespace vrock::ui
{
    class OpenGLBackend : public RenderBackend
    {
    public:
        OpenGLBackend( ) = default;

        auto init( ) -> bool final;
        auto open_window( WindowOptions options ) -> bool final;

    public:
        // private:
        GLFWwindow *window = nullptr;
    };
} // namespace vrock::ui