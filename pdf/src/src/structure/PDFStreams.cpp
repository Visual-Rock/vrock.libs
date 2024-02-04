#include "vrock/pdf/structure/PDFStreams.hpp"
#include "vrock/pdf/parser/PDFObjectParser.hpp"

#include "vrock/pdf/structure/PDFFilters.hpp"

#include <iomanip>

#include <vrock/utils/SpanHelpers.hpp>

namespace vrock::pdf
{
    PDFStream::PDFStream( std::shared_ptr<PDFDictionary> dictionary, in_data_t _data, PDFStreamType t )
        : PDFBaseObject( PDFObjectType::Stream ), stream_type( t ), dict( std::move( dictionary ) )
    {
        // get filters and param dictionary
        auto filter = dict->get( "Filter" );
        auto p = dict->get( "DecodeParms" );
        auto param = std::make_shared<PDFDictionary>( );
        if ( p->is( PDFObjectType::Dictionary ) )
            param = p->as<PDFDictionary>( );
        if ( auto name = filter->to<PDFName>( ) )
            filters.emplace_back( name->name );
        else if ( auto arr = filter->to<PDFArray>( ) )
        {
            for ( auto &item : arr->value )
                if ( auto name = item->to<PDFName>( ) )
                    filters.emplace_back( name->name );
        }

        // apply filters on data and save data
        data_t d = std::string( _data );

        for ( auto &f : filters )
            if ( encodings.contains( f ) )
                d = encodings[ f ]->decode( d, param );
        data = d;
    }

    PDFObjectStream::PDFObjectStream( std::shared_ptr<PDFDictionary> d, in_data_t data,
                                      std::shared_ptr<PDFContext> ctx )
        : PDFStream( std::move( d ), data, PDFStreamType::Object ), context( std::move( ctx ) )
    {
        parser = std::make_shared<PDFObjectParser>( data_t( this->data ) );
        parser->set_context( context );
        auto n = dict->get<PDFInteger>( "N" );
        auto f = dict->get<PDFInteger>( "First" );
        if ( !( n && f ) )
            throw std::runtime_error( "missing entries N and First in Object stream Dictionary" );
        first = f->value;
        for ( int32_t i = 0; i < n->value; i++ )
        {
            auto num = parser->parse_int( );
            offsets.emplace_back( num, parser->parse_int( ) );
        }
    }

    auto PDFObjectStream::get_object( std::size_t idx ) -> std::shared_ptr<PDFBaseObject>
    {
        auto p = offsets[ idx ];
        auto ref = std::make_shared<PDFRef>( p.first, 0, 1 );
        if ( objects.find( ref ) != objects.end( ) )
            return objects[ ref ];
        parser->_offset = p.second + first;
        objects[ ref ] = parser->parse_object( ref, false );
        return objects[ ref ];
    }

    auto PDFXRefStream::get_entries( ) -> std::unordered_map<std::shared_ptr<XRefEntry>, std::shared_ptr<XRefEntry>,
                                                             XRefEntryPtrHash, XRefEntryPtrEqual>
    {
        if ( auto size = dict->get<PDFInteger>( "Size" ) )
        {
            if ( auto w = dict->get<PDFArray>( "W" ) )
            {
                // Get field size
                std::size_t field_size[ 3 ];
                for ( int i = 0; i < 3; ++i )
                    if ( auto num = w->get<PDFInteger>( i ) )
                        field_size[ i ] = num->value;
                    else
                        throw std::runtime_error( "W entry of XRefStream incorrect" );

                // Get Indexes of subsections
                std::vector<std::int32_t> index = { 0, size->value };
                if ( auto indexes = dict->get<PDFArray>( "Index" ) )
                {
                    index.clear( );
                    index.reserve( indexes->value.size( ) );
                    for ( const auto &o : indexes->value )
                        if ( auto num = o->to<PDFInteger>( ) )
                            index.emplace_back( num->value );
                }

                // Parse entries
                std::unordered_map<std::shared_ptr<XRefEntry>, std::shared_ptr<XRefEntry>, XRefEntryPtrHash,
                                   XRefEntryPtrEqual>
                    entries = { };

                std::size_t offset = 0;
                std::size_t row_length = field_size[ 0 ] + field_size[ 1 ] + field_size[ 2 ];

                for ( std::size_t i = 0; i < index.size( ); i += 2 )
                {
                    auto start = index[ i ];
                    for ( auto j = 0; j < index[ i + 1 ]; ++j )
                    {
                        // read field 1
                        unsigned long field_1 = 0;
                        if ( field_size[ 0 ] != 0 )
                        {
                            auto d = utils::to_hex_string( data.substr( offset, field_size[ 0 ] ) );
                            field_1 = std::stoul( d, nullptr, 16 );
                        }
                        else
                            field_1 = 1;

                        // read field 2
                        auto d = utils::to_hex_string( data.substr( offset + field_size[ 0 ], field_size[ 1 ] ) );
                        auto field_2 = std::stoul( d, nullptr, 16 );

                        // read field 3
                        d = utils::to_hex_string(
                            data.substr( offset + field_size[ 0 ] + field_size[ 1 ], field_size[ 2 ] ) );
                        auto field_3 = std::stoul( d, nullptr, 16 );

                        auto e = std::make_shared<XRefEntry>( field_2, start + j, field_3, field_1 );
                        entries[ e ] = e;

                        offset += row_length;
                    }
                }
                return entries;
            }
            else
                throw std::runtime_error( "W not found in XRefStream Dictionary or it is not an Array" );
        }
        else
            throw std::runtime_error( "Size not found in XRefStream Dictionary or it is not an Integer" );
    }

    PDFXRefStream::PDFXRefStream( std::shared_ptr<PDFDictionary> d, in_data_t data )
        : PDFStream( std::move( d ), data, PDFStreamType::XRef )
    {
    }

    template <>
    auto to_object_type<PDFStream>( ) -> PDFObjectType
    {
        return PDFObjectType::Stream;
    }

    template <>
    auto to_stream_type<PDFStream>( ) -> PDFStreamType
    {
        return PDFStreamType::Raw;
    }

    template <>
    auto to_stream_type<PDFXRefStream>( ) -> PDFStreamType
    {
        return PDFStreamType::XRef;
    }

    template <>
    auto to_stream_type<PDFObjectStream>( ) -> PDFStreamType
    {
        return PDFStreamType::Object;
    }
} // namespace vrock::pdf