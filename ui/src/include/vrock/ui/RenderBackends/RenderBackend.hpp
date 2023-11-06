#pragma once

#include <cstdint>
#include <string>

namespace vrock::ui
{
    struct WindowOptions
    {
        std::uint16_t width, height;
        std::string window_title;
    };

    class RenderBackend
    {
    public:
        RenderBackend( ) = default;

        virtual auto init( ) -> bool = 0;
        virtual auto open_window( WindowOptions options ) -> bool = 0;

    private:
        WindowOptions options;
    };
} // namespace vrock::ui