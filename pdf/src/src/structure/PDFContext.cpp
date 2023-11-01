#include "vrock/pdf/structure/PDFContext.hpp"

#include "vrock/pdf/parser/PDFObjectParser.hpp"
#include "vrock/pdf/structure/PDFStreams.hpp"

namespace vrock::pdf
{
    XRefEntry::XRefEntry( std::uint32_t offset, std::uint32_t object_number, std::uint32_t gen_num, std::uint8_t type )
        : offset( offset ), object_number( object_number ), generation_number( gen_num ), type( type )
    {
    }

    size_t XRefEntryPtrHash::operator( )( const std::shared_ptr<XRefEntry> &r ) const
    {
        if ( r->type == 2 )
            return std::hash<std::uint64_t>( )( ( (uint64_t)r->object_number ) << 32 | 0 );
        return std::hash<std::uint64_t>( )( ( (uint64_t)r->object_number ) << 32 | r->generation_number );
    }

    bool XRefEntryPtrEqual::operator( )( const std::shared_ptr<XRefEntry> &l,
                                         const std::shared_ptr<XRefEntry> &r ) const
    {
        if ( r->type == 2 )
            return l->object_number == r->object_number;
        return l->object_number == r->object_number && l->generation_number == r->generation_number;
    }

    auto XRefTable::get_entry( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<XRefEntry>
    {
        auto entry = std::make_shared<XRefEntry>( 0, ref->object_number, ref->generation_number, 1 );
        if ( entries.contains( entry ) )
        {
            auto e = entries[ entry ];
            if ( e->type != 0 )
                return e;
        }
        return nullptr;
    }

    PDFContext::PDFContext( std::shared_ptr<PDFObjectParser> parser ) : parser( std::move( parser ) )
    {
    }

    auto PDFContext::init( ) -> void
    {
        xref_tables = std::move( parser->parse_xref( ) );
        trailer = xref_tables[ 0 ]->trailer;
    }

    auto PDFContext::get_object( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<PDFBaseObject>
    {
        if ( ref == nullptr )
            return nullptr;
        if ( objects.contains( ref ) )
            return objects[ ref ];

        std::shared_ptr<XRefEntry> entry = nullptr;
        for ( const auto &table : xref_tables )
        {
            entry = table->get_entry( ref );
            if ( entry )
                break;
        }

        if ( entry )
        {
            std::shared_ptr<PDFBaseObject> obj;

            switch ( entry->type )
            {
            case 0:
                obj = std::make_shared<PDFNull>( );
                break;
            case 1: {
                parser->_offset = entry->offset;
                if ( auto ref = parser->parse_object( nullptr, false )->to<PDFRef>( ) )
                    obj = parser->parse_object( ref, true );
                else
                    throw std::runtime_error( "failed to parse object reference" );
                break;
            }
            case 2: {
                // get stream + convert
                if ( auto objstm = get_object<PDFStream>( std::make_shared<PDFRef>( entry->offset, 0, 1 ) )
                                       ->to_stream<PDFObjectStream>( ) )
                    obj = objstm->get_object( entry->generation_number );
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
} // namespace vrock::pdf