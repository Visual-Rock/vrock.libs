#pragma once

#include "UIElement.hpp"

namespace vrock::ui
{
    class TextBlock : public UIElement
    {
    public:


        auto render( const ::vrock::ui::RenderBackend &renderer ) -> void override;
    };
} // namespace vrock::ui