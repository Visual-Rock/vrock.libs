#pragma once

#include "../RenderBackends/RenderBackend.hpp"

namespace vrock::ui
{
    class UIElement
    {
    public:
        UIElement( ) = default;

        virtual auto render( const RenderBackend &renderer ) -> void = 0;
    };
} // namespace vrock::ui