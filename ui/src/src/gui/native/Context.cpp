#include "vrock/ui/gui/native/Context.hpp"

#include <include/gpu/gl/GrGLAssembleInterface.h>
#include <include/gpu/gl/GrGLInterface.h>

#ifdef _WIN32
#include <windows.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <GL/glext.h>

namespace vrock::ui
{
    GraphicsContext::~GraphicsContext( ) = default;

    OpenGlContext::OpenGlContext( gr_gl_get_function_t fn )
        : GraphicsContext{ }, interface{ GrGLAssembleInterface( nullptr, fn ) }
    {
    }

    OpenGlContext::~OpenGlContext( ) = default;

    std::unique_ptr<Canvas> OpenGlContext::make_canvas( const Point &size, bool anti_alias )
    {
        glViewport( 0, 0, size.width, size.height );
        return std::make_unique<OpenGlCanvas>( size, interface.get( ), anti_alias );
    }
} // namespace vrock::ui