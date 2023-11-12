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
        NativeWindow( std::unique_ptr<NativeVisual> native_visual );
        virtual ~NativeWindow( ) = 0;

        virtual auto create( const Point &position, const Point &size, bool show_title_bar ) -> void = 0;

        virtual auto show( ) -> void = 0;
        virtual auto hide( ) -> void = 0;
        virtual auto close( ) -> void = 0;

        virtual auto get_title( ) -> std::string = 0;
        virtual auto set_title( const std::string &title ) -> void = 0;

        void make_current( ) const;
        void swap_buffers( const Point &size );

        NativeVisual &get_native_visual( );
        [[nodiscard]] const NativeVisual &get_native_visual( ) const;

    protected:
        std::unique_ptr<NativeVisual> native_visual;
    };
} // namespace vrock::ui