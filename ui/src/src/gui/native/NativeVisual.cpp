#include "vrock/ui/gui/native/NativeVisual.hpp"

namespace vrock::ui
{
    NativeVisual::~NativeVisual( ) = default;

    NativeVisual::gl_get_function_t NativeVisual::get_gl_function( ) const
    {
        return nullptr;
    }
} // namespace vrock::ui