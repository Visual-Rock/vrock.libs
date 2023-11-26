#include "vrock/pdf/structure/Functions.hpp"

namespace vrock::pdf
{
    Function::Function( std::shared_ptr<PDFDictionary> _dict ) : dict( std::move( _dict ) )
    {
        if ( auto t = dict->get<PDFNumber>( "FunctionType" ) )
            type = static_cast<FunctionType>( t->as_int( ) );

        // Domain
        if ( auto d = dict->get<PDFArray>( "Domain" ) )
        {
            m = d->value.size( ) / 2;
            domain.resize( m );
            for ( int i = 0; i < m; ++i )
            {
                if ( auto num1 = d->get<PDFNumber>( 2 * i ) )
                    domain[ i ].first = num1->as_double( );
                if ( auto num2 = d->get<PDFNumber>( 2 * i + 1 ) )
                    domain[ i ].second = num2->as_double( );
            }
        }

        // Range
        if ( auto r = dict->get<PDFArray>( "Range" ) )
        {
            n = r->value.size( ) / 2;
            for ( int i = 0; i < n; ++i )
            {
                range.emplace_back( );
                if ( auto num1 = r->get<PDFNumber>( 2 * i ) )
                    domain[ i ].first = num1->as_double( );
                if ( auto num2 = r->get<PDFNumber>( 2 * i + 1 ) )
                    domain[ i ].second = num2->as_double( );
            }
        }
    }

    SampledFunction::SampledFunction( std::shared_ptr<PDFStream> stream ) : Function( stream->dict )
    {
        // Size
        if ( auto size_arr = stream->dict->get<PDFArray>( "Size" ) )
            for ( auto &e : size_arr->value )
                if ( auto num = e->to<PDFNumber>( ) )
                    size.emplace_back( num->as_int( ) );

        // Offsets
        offsets.reserve( 1 << m );
        std::int32_t idx, bit, j, t;
        for ( int i = 0; i < ( 1 << m ); ++i )
        {
            idx = 0;
            for ( j = m - 1, t = i; j >= 1; --j, j <<= 1 )
            {
                if ( size[ j ] == 1 )
                    bit = 0;
                else
                    bit = ( t >> ( m - 1 ) ) & 1;
                idx = ( idx + bit ) * size[ j - 1 ];
            }
            if ( m > 0 && size[ 0 ] == 1 )
                bit = 0;
            else
                bit = ( t >> ( m - 1 ) ) & 1;
            offsets[ i ] = ( idx + bit ) * n;
        }

        // BitsPerSample
        if ( auto bps = stream->dict->get<PDFNumber>( "BitsPerSample" ) )
            bits_per_sample = bps->as_int( );

        // Encode
        if ( auto e = dict->get<PDFArray>( "Encode" ) )
        {
            encode.resize( m );
            for ( int i = 0; i < m; ++i )
            {
                if ( auto num1 = e->get<PDFNumber>( 2 * i ) )
                    encode[ i ].first = num1->as_double( );
                if ( auto num2 = e->get<PDFNumber>( 2 * i + 1 ) )
                    encode[ i ].second = num2->as_double( );
            }
        }
        else
        {
            encode.resize( m );
            for ( int i = 0; i < m; ++i )
            {
                encode[ i ].first = 0.0;
                encode[ i ].second = size[ i ] - 1;
            }
        }

        // input multipliers
        input_multipliers.resize( m, 0.0 );
        for ( int i = 0; i < m; ++i )
            input_multipliers[ i ] =
                ( encode[ i ].second - encode[ i ].first ) / ( domain[ i ].second - domain[ i ].first );

        // Decode
        if ( auto d = dict->get<PDFArray>( "Decode" ) )
        {
            decode.resize( n );
            for ( int i = 0; i < n; ++i )
            {
                if ( auto num1 = d->get<PDFNumber>( 2 * i ) )
                    decode[ i ].first = num1->as_double( );
                if ( auto num2 = d->get<PDFNumber>( 2 * i + 1 ) )
                    decode[ i ].second = num2->as_double( );
            }
        }
        else
            for ( int i = 0; i < n; ++i )
                decode[ i ] = range[ i ];
    }

    auto SampledFunction::sample( std::vector<float> numbers ) -> std::vector<float>
    {

        return { };
    }

    auto make_function( std::shared_ptr<PDFBaseObject> object ) -> std::shared_ptr<Function>
    {
        if ( auto stream = object->to<PDFStream>( ) )
        {
            if ( auto num = stream->dict->get<PDFNumber>( "FunctionType" ) )
            {
                switch ( num->as_int( ) )
                {
                case 0:
                    return std::make_shared<SampledFunction>( stream );
                }
            }
        }
    }
} // namespace vrock::pdf