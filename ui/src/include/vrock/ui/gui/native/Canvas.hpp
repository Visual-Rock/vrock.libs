#pragma once

#include <memory>

#include <vrock/ui/Point.hpp>

class SkSurface;

struct GrGLInterface;
class GrDirectContext;

namespace vrock::ui
{
    class Canvas
    {
    public:
        explicit Canvas( bool anti_alias );
        ~Canvas( ) = default;

        std::unique_ptr<SkSurface> surface;

    protected:
        bool anti_alias;
    };

    class OpenGlCanvas : public Canvas
    {
    public:
        OpenGlCanvas( const Point &size, const GrGLInterface *interface, bool anti_alias );
        ~OpenGlCanvas( );

    private:
        std::unique_ptr<GrDirectContext> context;
    };
} // namespace vrock::ui