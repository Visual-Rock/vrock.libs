#pragma once

#include "gui/Window.hpp"

namespace vrock::ui
{
    struct ApplicationSettings
    {
        WindowSettings window;
    };

    class Application
    {
    public:
        explicit Application( ApplicationSettings settings );

        auto run( ) -> int;

        std::unique_ptr<Window> window;

    private:
        ApplicationSettings settings;
    };
} // namespace vrock::ui
