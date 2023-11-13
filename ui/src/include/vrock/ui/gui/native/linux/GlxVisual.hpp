#pragma once

#include "../NativeVisual.hpp"

#include <GL/glx.h>
#include <memory>

namespace vrock::ui
{
    class GlxVisual : public NativeVisual
    {
    private:
        struct xfree_deleter
        {
            void operator( )( void *x_data );
        };

    public:
        using x_visual_info_t = std::unique_ptr<XVisualInfo, xfree_deleter>;

        GlxVisual( Display *display );
        ~GlxVisual( ) override;

        auto create_surface( std::uintptr_t window ) -> void override;
        auto make_current( ) const -> void override;
        auto swap_buffers( const Point &size ) const -> void override;

        auto get_xvisualinfo( ) -> XVisualInfo *;

        gl_get_function_t get_gl_function( ) const override;

    private:
        auto create_xvisualinfo( ) const -> x_visual_info_t;

        Display *display;
        x_visual_info_t info;
        GLXContext context;
        GLXDrawable drawable;
    };

} // namespace vrock::ui