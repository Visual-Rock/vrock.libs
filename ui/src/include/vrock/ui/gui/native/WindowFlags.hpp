#pragma once

namespace vrock::ui
{
    enum class WindowFlags : char
    {
        ExitOnClose = 0b0001, // Quit application when last window with this flag set closes
        AntiAlias = 0b0010,   // enable anti-aliased rendering for window content
        OpenGL = 0b0100,      // use OpenGL to render window content
        _None = 0b1000,
    };

    auto inline has_flag( const WindowFlags &flags, WindowFlags flag ) -> bool
    {
        return (int)flags & (int)flag;
    }
} // namespace vrock::ui