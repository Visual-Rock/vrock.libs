#include "vrock/ui/gui/native/Canvas.hpp"

#include <include/gpu/gl/GrGLInterface.h>

#include "include/core/SkImage.h"
#include "include/core/SkSurface.h"
#include <include/core/SkCanvas.h>
#include <include/gpu/GrDirectContext.h>

namespace vrock::ui
{
    Canvas::Canvas( bool anti_alias ) : anti_alias( anti_alias ), surface{ nullptr }
    {
    }

    OpenGlCanvas::OpenGlCanvas( const Point &size, const GrGLInterface *interface, bool anti_alias )
        : Canvas( anti_alias ), context( GrDirectContext::MakeGL( sk_sp{ interface } ).release( ) )
    {
        SkImageInfo info = SkImageInfo::MakeN32Premul( size.width, size.height );
        surface = std::unique_ptr<SkSurface>(
            SkSurface::MakeRenderTarget( context.get( ), SkBudgeted::kNo, info ).release( ) );
    }

    OpenGlCanvas::~OpenGlCanvas( )
    {
        surface.reset( );
    }
} // namespace vrock::ui