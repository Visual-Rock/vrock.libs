#pragma once

#include "PDFDataStructures.hpp"
#include "PDFObjects.hpp"

namespace vrock::pdf
{
    class Rectangle : public PDFBaseObject
    {
    public:
        explicit Rectangle( std::shared_ptr<PDFArray> arr );

        Point lower_left;
        Point upper_left;
        Point upper_right;
        Point lower_right;

        auto get_width( ) const -> Unit;
        auto get_height( ) const -> Unit;
    };
} // namespace vrock::pdf