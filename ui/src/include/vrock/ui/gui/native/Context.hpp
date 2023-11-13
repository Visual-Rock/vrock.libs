#pragma once

#include "Canvas.hpp"

namespace vrock::ui
{
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext( );

        virtual std::unique_ptr<Canvas> make_canvas( const Point &size, bool anti_alias ) = 0;
    };

    class OpenGlContext : public GraphicsContext
    {
    public:
        using gl_function_t = void ( * )( );
        using gr_gl_get_function_t = gl_function_t ( * )( void *, const char[] );

        explicit OpenGlContext( gr_gl_get_function_t fn );
        ~OpenGlContext( ) override;

        std::unique_ptr<Canvas> make_canvas( const Point &size, bool anti_alias ) override;

    private:
        std::unique_ptr<const GrGLInterface> interface;
    };
} // namespace vrock::ui
