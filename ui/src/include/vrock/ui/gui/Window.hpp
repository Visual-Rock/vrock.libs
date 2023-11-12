#pragma once

#include <memory>

#include "native/NativeWindow.hpp"

namespace vrock::ui
{
    class Window
    {
    public:
        ~Window( );

    private:
        std::unique_ptr<NativeWindow> native_window = nullptr;
    };
} // namespace vrock::ui