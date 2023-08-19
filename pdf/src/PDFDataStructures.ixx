export module vrock.pdf.PDFDataStructures;

namespace vrock::pdf
{
    export class Unit
    {
    public:
        explicit Unit( double u = 0.0 ) : units( u )
        {
        }

        ~Unit( ) = default;

        [[nodiscard]] auto inline to_inch( ) const -> double
        {
            return units / 72;
        }

        [[nodiscard]] auto inline to_cm( ) const -> double
        {
            return to_inch( ) * 2.54;
        }

        double units = 0.0;
    };

    export struct Point
    {
        Unit x, y;
    };
} // namespace vrock::pdf