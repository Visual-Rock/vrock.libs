module;

#include <array>

export module vrock.pdf.Math;

export namespace vrock::pdf
{
    typedef std::array<std::array<double, 3>, 3> mat3;

    auto mul( const mat3 &a, const mat3 &b ) -> mat3
    {
        mat3 res{ { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } };

        res[ 0 ][ 0 ] = a[ 0 ][ 0 ] * b[ 0 ][ 0 ] + a[ 0 ][ 1 ] * b[ 1 ][ 0 ] + a[ 0 ][ 2 ] * b[ 2 ][ 0 ];
        res[ 1 ][ 0 ] = a[ 1 ][ 0 ] * b[ 0 ][ 0 ] + a[ 1 ][ 1 ] * b[ 1 ][ 0 ] + a[ 1 ][ 2 ] * b[ 2 ][ 0 ];
        res[ 2 ][ 0 ] = a[ 2 ][ 0 ] * b[ 0 ][ 0 ] + a[ 2 ][ 1 ] * b[ 1 ][ 0 ] + a[ 2 ][ 2 ] * b[ 2 ][ 0 ];

        res[ 0 ][ 1 ] = a[ 0 ][ 0 ] * b[ 0 ][ 1 ] + a[ 0 ][ 1 ] * b[ 1 ][ 1 ] + a[ 0 ][ 2 ] * b[ 2 ][ 1 ];
        res[ 1 ][ 1 ] = a[ 1 ][ 0 ] * b[ 0 ][ 1 ] + a[ 1 ][ 1 ] * b[ 1 ][ 1 ] + a[ 1 ][ 2 ] * b[ 2 ][ 1 ];
        res[ 2 ][ 1 ] = a[ 2 ][ 0 ] * b[ 0 ][ 1 ] + a[ 2 ][ 1 ] * b[ 1 ][ 1 ] + a[ 2 ][ 2 ] * b[ 2 ][ 1 ];

        res[ 0 ][ 2 ] = a[ 0 ][ 0 ] * b[ 0 ][ 2 ] + a[ 0 ][ 1 ] * b[ 1 ][ 2 ] + a[ 0 ][ 2 ] * b[ 2 ][ 2 ];
        res[ 1 ][ 2 ] = a[ 1 ][ 0 ] * b[ 0 ][ 2 ] + a[ 1 ][ 1 ] * b[ 1 ][ 2 ] + a[ 1 ][ 2 ] * b[ 2 ][ 2 ];
        res[ 2 ][ 2 ] = a[ 2 ][ 0 ] * b[ 0 ][ 2 ] + a[ 2 ][ 1 ] * b[ 1 ][ 2 ] + a[ 2 ][ 2 ] * b[ 2 ][ 2 ];

        return res;
    }
} // namespace vrock::pdf