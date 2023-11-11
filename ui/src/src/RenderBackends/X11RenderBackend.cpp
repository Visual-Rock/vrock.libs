#include "vrock/ui/RenderBackends/X11Renderbackend.hpp"

namespace vrock::ui
{
    auto X11RenderBackend::init( ) -> bool
    {
        return true;
    }
    auto X11RenderBackend::open_window( WindowOptions _options ) -> bool
    {
        options = std::move( _options );

        return true;
    }
} // namespace vrock::ui