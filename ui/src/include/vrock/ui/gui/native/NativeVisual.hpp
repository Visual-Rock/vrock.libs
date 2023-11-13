#pragma once

#include <cstdint>

#include "vrock/ui/Point.hpp"

namespace vrock::ui
{
    class NativeVisual
    {
    public:
        NativeVisual( ) = default;
        virtual ~NativeVisual( ) = 0;

        virtual auto create_surface( std::uintptr_t window ) -> void = 0;
        virtual auto make_current( ) const -> void = 0;
        virtual auto swap_buffers( const Point &size ) const -> void = 0;

        using gl_function_t = void ( * )( );
        using gl_get_function_t = gl_function_t ( * )( void *ctx, const char name[] );

        virtual gl_get_function_t get_gl_function( ) const;
    };
} // namespace vrock::ui