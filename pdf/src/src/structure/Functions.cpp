#include "vrock/pdf/structure/Functions.hpp"

namespace vrock::pdf
{
    Function::Function( std::shared_ptr<PDFDictionary> dict ) : dict( std::move( dict ) )
    {
        if ( auto t = dict->get<PDFNumber>( "FunctionType" ) )
            type = static_cast<FunctionType>( t->as_int( ) );
        // Domain
        if ( auto d = dict->get<PDFArray>( "Domain" ) )
            for ( auto &e : d->value )
                if ( auto n = e->to<PDFNumber>( ) )
                    domain.push_back( n );
        // Range
        if ( auto d = dict->get<PDFArray>( "Range" ) )
            for ( auto &e : d->value )
                if ( auto n = e->to<PDFNumber>( ) )
                    domain.push_back( n );
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