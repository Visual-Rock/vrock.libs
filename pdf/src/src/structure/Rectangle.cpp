#include "vrock/pdf/structure/Rectangle.hpp"

namespace vrock::pdf
{
    Rectangle::Rectangle( std::shared_ptr<PDFArray> arr ) : PDFBaseObject( PDFObjectType::Rectangle )
    {
        if ( arr->value.size( ) != 4 )
            throw std::runtime_error( "invalid Rectangle" );
        lower_left = { Unit( arr->get( 0 )->to<PDFNumber>( )->as_double( ) ),
                       Unit( arr->get( 1 )->to<PDFNumber>( )->as_double( ) ) };

        upper_right = { Unit( arr->get( 2 )->to<PDFNumber>( )->as_double( ) ),
                        Unit( arr->get( 3 )->to<PDFNumber>( )->as_double( ) ) };

        upper_left = { lower_left.x, upper_right.y };
        lower_right = { upper_right.x, lower_left.y };
    }

    auto Rectangle::get_width( ) const -> Unit
    {
        return Unit( upper_right.x.units - lower_left.x.units );
    }

    auto Rectangle::get_height( ) const -> Unit
    {
        return Unit( upper_right.y.units - lower_left.y.units );
    }
} // namespace vrock::pdf