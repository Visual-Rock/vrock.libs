module;

import vrock.utils.ByteArray;

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

module vrock.pdf.PDFBaseObjects;

import vrock.pdf.PDFObjectParser;

namespace vrock::pdf
{
    auto PDFDictionary::get( const std::string &k, bool resolve ) -> std::shared_ptr<PDFBaseObject>
    {
        auto key = std::make_shared<PDFName>( k );
        if ( dict.find( key ) == dict.end( ) )
            return nullptr;
        if ( resolve && dict[ key ]->type == PDFObjectType::IndirectObject )
        {
            auto ref = dict[ key ]->as<PDFRef>( );
            return context->get_object( ref );
        }
        return dict[ key ];
    }

    // PDFStreams

    auto PDFXRefStream::get_entries( ) -> std::vector<std::shared_ptr<XRefEntry>>
    {
        if ( auto size = dict->get<PDFInteger, PDFObjectType::Integer>( "Size" ) )
        {
            if ( auto w = dict->get<PDFArray, PDFObjectType::Array>( "W" ) )
            {
                // Get field size
                std::size_t field_size[ 3 ];
                for ( int i = 0; i < 3; ++i )
                    if ( auto num = w->get<PDFInteger, PDFObjectType::Integer>( i ) )
                        field_size[ i ] = num->value;
                    else
                        throw std::runtime_error( "W entry of XRefStream incorrect" );

                // Get Indexes of subsections
                std::vector<std::int32_t> index = { 0, size->value };
                if ( auto indexes = dict->get<PDFArray, PDFObjectType::Array>( "Index" ) )
                {
                    index.clear( );
                    index.reserve( indexes->value.size( ) );
                    for ( const auto &o : indexes->value )
                        if ( auto num = o->to<PDFInteger, PDFObjectType::Integer>( ) )
                            index.emplace_back( num->value );
                }

                // Parse entries
                std::vector<std::shared_ptr<XRefEntry>> entries = { };
                std::size_t offset = 0;
                std::size_t row_length = field_size[ 0 ] + field_size[ 1 ] + field_size[ 2 ];

                for ( std::size_t i = 0; i < index.size( ); i += 2 )
                {
                    auto start = index[ i ];
                    for ( auto j = 0; j < index[ i + 1 ]; ++j )
                    {
                        std::stringstream field;
                        // read field 1
                        unsigned long field_1 = 0;
                        if ( field_size[ 0 ] != 0 )
                        {
                            for ( size_t l = 0; l < field_size[ 0 ]; ++l )
                                field << std::setw( 2 ) << std::setfill( '0' ) << std::hex
                                      << (int)data->data( )[ offset + l ];
                            field_1 = std::stoul( field.str( ), nullptr, 16 );
                            field.str( "" );
                        }
                        else
                            field_1 = 1;

                        // read field 2
                        for ( size_t l = 0; l < field_size[ 1 ]; ++l )
                            field << std::setw( 2 ) << std::setfill( '0' ) << std::hex
                                  << (int)data->data( )[ offset + field_size[ 0 ] + l ];
                        auto field_2 = std::stoul( field.str( ), nullptr, 16 );
                        field.str( "" );
                        // read field 3
                        for ( size_t l = 0; l < field_size[ 2 ]; ++l )
                            field << std::setw( 2 ) << std::setfill( '0' ) << std::hex
                                  << (int)data->data( )[ offset + field_size[ 0 ] + field_size[ 1 ] + l ];
                        auto field_3 = std::stoul( field.str( ), nullptr, 16 );

                        switch ( field_1 )
                        {
                        case 0:
                            entries.push_back( std::make_shared<XRefEntry>( field_2, start + j, field_3, field_1 ) );
                            break;
                        case 1:
                            entries.push_back( std::make_shared<XRefEntry>( field_2, start + j, field_3, field_1 ) );
                            break;
                        case 2:
                            entries.push_back( std::make_shared<XRefEntry>( field_3, field_2, start + j, field_1 ) );
                            break;
                            // default:
                            // log::get_logger( "pdf" )->log->info( "unknown value for field 1, value: {}", field_1 );
                        }
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
    } // namespace vrock::pdf

    PDFObjectStream::PDFObjectStream( std::shared_ptr<PDFDictionary> d, std::shared_ptr<utils::ByteArray<>> data,
                                      std::shared_ptr<PDFContext> ctx )
        : PDFStream( std::move( d ), std::move( data ), PDFStreamType::Object ), context( std::move( ctx ) )
    {
        parser = std::make_shared<PDFObjectParser>( this->data->to_string( ) );
        parser->set_context( context );
        auto n = dict->get<PDFInteger, PDFObjectType::Integer>( "N" );
        auto f = dict->get<PDFInteger, PDFObjectType::Integer>( "First" );
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

    // PDFContext

    PDFContext::PDFContext( std::shared_ptr<PDFObjectParser> parser ) : parser( std::move( parser ) )
    {
    }

    auto PDFContext::init( ) -> void
    {
        xref_tables = std::move( parser->parse_xref( ) );
        trailer = xref_tables[ 0 ]->trailer;
    }

    auto get_entry( const std::vector<std::shared_ptr<XRefTable>> &xref_tables, std::shared_ptr<PDFRef> ref )
        -> std::shared_ptr<XRefEntry>
    {
        for ( const auto &xref : xref_tables ) // first is newest xref table
            for ( auto e : xref->entries )
                if ( e->type == 1 && e->object_number == ref->object_number ) // normal indirect Object
                    return e;
                else if ( e->type == 2 && e->generation_number == ref->object_number ) // Object inside Object stream
                    return e;
        return nullptr;
    }

    auto PDFContext::get_object( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<PDFBaseObject>
    {
        if ( ref == nullptr )
            return nullptr;
        if ( objects.contains( ref ) )
            return objects[ ref ];

        // Parse Object from Xref
        if ( auto entry = get_entry( xref_tables, ref ) )
        {
            std::shared_ptr<PDFBaseObject> obj;
            switch ( entry->type )
            {
            case 0:
                obj = std::make_shared<PDFNull>( );
                break;
            case 1: {
                parser->_offset = entry->offset;
                if ( auto ref = parser->parse_object( nullptr, false )->to<PDFRef, PDFObjectType::IndirectObject>( ) )
                    obj = parser->parse_object( ref, true );
                else
                    throw std::runtime_error( "failed to parse object reference" );
                break;
            }
            case 2: {
                // get stream + convert
                if ( auto objstm = get_object<PDFStream, PDFObjectType::Stream>(
                                       std::make_shared<PDFRef>( entry->object_number, 0, 1 ) )
                                       ->to_stream<PDFObjectStream, PDFStreamType::Object>( ) )
                    obj = objstm->get_object( entry->offset );
                break;
            }
            default:
                obj = std::make_shared<PDFNull>( );
            }
            objects[ ref ] = obj;
            return obj;
        }
        return nullptr;
    }

    template <typename T, PDFObjectType ObjType>
        requires std::is_base_of_v<PDFBaseObject, T>
    auto PDFContext::get_object( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<T>
    {
        return get_object( ref )->to<T, ObjType>( );
    }
} // namespace vrock::pdf