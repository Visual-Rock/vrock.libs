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
            for ( int i = 0; i < m; ++i )
            {
                domain.emplace_back( );
                if ( auto num1 = d->get<PDFNumber>( 2 * i ) )
                    domain[ i ][ 0 ] = num1->as_double( );
                if ( auto num2 = d->get<PDFNumber>( 2 * i + 1 ) )
                    domain[ i ][ 1 ] = num2->as_double( );
                std::cout << domain[ i ][ 0 ] << " " << domain[ i ][ 1 ] << std::endl;
            }
        }
        //            for ( auto &e : d->value )
        //                if ( auto n = e->to<PDFNumber>( ) )
        //                    domain.push_back( n );
        // Range
        //        if ( auto d = dict->get<PDFArray>( "Range" ) )
        //            for ( auto &e : d->value )
        //                if ( auto n = e->to<PDFNumber>( ) )
        //                    domain.push_back( n );
    }

    SampledFunction::SampledFunction( std::shared_ptr<PDFStream> stream ) : Function( stream->dict )
    {
        if ( auto size_arr = stream->dict->get<PDFArray>( "Size" ) )
            for ( auto &e : size_arr->value )
                if ( auto num = e->to<PDFNumber>( ) )
                    size.emplace_back( num->as_int( ) );
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