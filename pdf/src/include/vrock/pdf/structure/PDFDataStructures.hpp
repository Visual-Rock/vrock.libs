#pragma once

namespace vrock::pdf
{
    class Unit
    {
    public:
        explicit Unit( float u = 0.0 );

        ~Unit( ) = default;

        [[nodiscard]] auto inline to_inch( ) const -> double
        {
            return units / 72;
        }

        [[nodiscard]] auto inline to_cm( ) const -> double
        {
            return to_inch( ) * 2.54;
        }

        float units = 0.0;
    };

    struct Point
    {
        Unit x, y;
    };
} // namespace vrock::pdf