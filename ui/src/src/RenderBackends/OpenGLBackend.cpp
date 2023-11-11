#include "vrock/ui/RenderBackends/OpenGLBackend.hpp"

#include <cmath>
#include <vector>

#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"

namespace vrock::ui
{
    bool OpenGLBackend::init( )
    {
        return true;
    }

    bool OpenGLBackend::open_window( WindowOptions _options )
    {
        sk_sp<const GrGLInterface> interface = nullptr;

        return true;
    }
} // namespace vrock::ui