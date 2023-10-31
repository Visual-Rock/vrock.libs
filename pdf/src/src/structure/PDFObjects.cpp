#include "vrock/pdf/structure/PDFObjects.hpp"

#include "vrock/pdf/structure/PDFContext.hpp"

#include <sstream>

namespace vrock::pdf
{
    PDFBaseObject::PDFBaseObject( PDFObjectType t ) : type( t )
    {
    }

    PDFRef::PDFRef( uint32_t obj_num, uint32_t gen_num, uint8_t type )
        : PDFBaseObject( PDFObjectType::IndirectObject ), object_number( obj_num ), generation_number( gen_num ),
          type( type )
    {
        tag = std::format( "{} {} R", object_number, generation_number );
    }

    PDFName::PDFName( std::string n, bool parse ) : PDFBaseObject( PDFObjectType::Name )
    {
        if ( parse )
        {
            std::stringstream ss;
            for ( size_t i = 0; i < n.length( ); i++ )
            {
                if ( n[ i ] != '#' )
                    ss.write( &n[ i ], 1 );
                else
                {
                    char c = (char)std::stoul( n.substr( ++i, 2 ), nullptr, 16 );
                    ss.write( &c, 1 );
                    i++;
                }
            }
            name = ss.str( );
        }
        else
            name = n;
    }

    auto PDFArray::get( std::size_t idx, bool resolve ) -> std::shared_ptr<PDFBaseObject>
    {
        if ( value.size( ) < idx )
            return nullptr;
        if ( resolve && value[ idx ]->type == PDFObjectType::IndirectObject )
        {
            auto ref = value[ idx ]->as<PDFRef>( );
            return context->get_object( ref );
        }
        return value[ idx ];
    }

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

    template <>
    auto to_object_type<PDFNull>( ) -> PDFObjectType
    {
        return PDFObjectType::Null;
    }

    template <>
    auto to_object_type<PDFRef>( ) -> PDFObjectType
    {
        return PDFObjectType::IndirectObject;
    }

    template <>
    auto to_object_type<PDFBool>( ) -> PDFObjectType
    {
        return PDFObjectType::Bool;
    }

    template <>
    auto to_object_type<PDFNumber>( ) -> PDFObjectType
    {
        return PDFObjectType::Number;
    }

    template <>
    auto to_object_type<PDFInteger>( ) -> PDFObjectType
    {
        return PDFObjectType::Number;
    }

    template <>
    auto to_object_type<PDFReal>( ) -> PDFObjectType
    {
        return PDFObjectType::Number;
    }

    template <>
    auto to_object_type<PDFString>( ) -> PDFObjectType
    {
        return PDFObjectType::String;
    }

    template <>
    auto to_object_type<PDFName>( ) -> PDFObjectType
    {
        return PDFObjectType::Name;
    }

    template <>
    auto to_object_type<PDFArray>( ) -> PDFObjectType
    {
        return PDFObjectType::Array;
    }

    template <>
    auto to_object_type<PDFDictionary>( ) -> PDFObjectType
    {
        return PDFObjectType::Dictionary;
    }
} // namespace vrock::pdf