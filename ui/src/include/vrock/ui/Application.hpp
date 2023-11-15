#pragma once

namespace vrock::ui
{
    struct ApplicationSettings
    {
    };

    class Application
    {
    public:
        explicit Application( ApplicationSettings settings );

        auto run( ) -> int;

    private:
        ApplicationSettings settings;
    };
} // namespace vrock::ui
