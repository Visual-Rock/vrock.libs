#pragma once

#include <memory>
#include <string>

#include "NativeVisual.hpp"
#include "vrock/ui/Point.hpp"

namespace vrock::ui
{
    class NativeWindow
    {
    public:
        NativeWindow( ) = default;

        virtual auto create( const Point &position, const Point &size, bool show_title_bar ) -> void = 0;

        virtual auto show( ) -> void = 0;
        virtual auto hide( ) -> void = 0;
        virtual auto close( ) -> void = 0;

        virtual auto get_title( ) -> std::string = 0;
        virtual auto set_title( const std::string &title ) -> void = 0;

    protected:
        std::unique_ptr<NativeVisual> native_visual;
    };
} // namespace vrock::ui