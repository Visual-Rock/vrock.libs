#include "vrock/pdf/structure/PDFObjects.hpp"

#include "vrock/pdf/structure/PDFContext.hpp"

#include <sstream>

namespace vrock::pdf
{
    PDFBaseObject::~PDFBaseObject( ) = default;

    PDFBaseObject::PDFBaseObject( PDFObjectType t ) : type( t )
    {
    }

    PDFRef::PDFRef( uint32_t obj_num, uint32_t gen_num, uint8_t type )
        : PDFBaseObject( PDFObjectType::IndirectObject ), object_number( obj_num ), generation_number( gen_num ),
          type( type )
    {
        tag = std::format( "{} {} R", object_number, generation_number );
    }

    bool PDFRef::operator==( const PDFRef &rhs ) const
    {
        return object_number == rhs.object_number && generation_number == rhs.generation_number;
    }

    bool PDFRef::operator!=( const PDFRef &rhs ) const
    {
        return !( rhs == *this );
    }

    size_t PDFRefPtrHash::operator( )( const std::shared_ptr<PDFRef> &r ) const
    {
        uint64_t l = ( (uint64_t)r->object_number ) << 32 | r->generation_number;
        return std::hash<uint64_t>( )( l );
    }

    bool PDFRefPtrEqual::operator( )( const std::shared_ptr<PDFRef> &l, const std::shared_ptr<PDFRef> &r ) const
    {
        return l->object_number == r->object_number && l->generation_number == r->generation_number;
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
    bool PDFName::operator==( const PDFName &rhs ) const
    {
        return name == rhs.name;
    }

    bool PDFName::operator!=( const PDFName &rhs ) const
    {
        return !( rhs == *this );
    }

    size_t PDFNamePtrHash::operator( )( const std::shared_ptr<PDFName> &n ) const
    {
        return std::hash<std::string>( )( n->name );
    }

    bool PDFNamePtrEqual::operator( )( const std::shared_ptr<PDFName> &l, const std::shared_ptr<PDFName> &r ) const
    {
        return l->name == r->name;
    }

    PDFBool::PDFBool( bool b ) : PDFBaseObject( PDFObjectType::Bool ), value( b )
    {
    }

    PDFArray::PDFArray( std::shared_ptr<PDFContext> ctx, std::vector<std::shared_ptr<PDFBaseObject>> v )
        : PDFBaseObject( PDFObjectType::Array ), value( std::move( v ) ), context( std::move( ctx ) )
    {
    }

    auto PDFArray::get( std::size_t idx, bool resolve ) -> std::shared_ptr<PDFBaseObject>
    {
        if ( value.size( ) < idx )
            return nullptr;
        if ( resolve && value[ idx ]->type == PDFObjectType::IndirectObject )
        {
            auto ref = value[ idx ]->to<PDFRef>( );
            return context->get_object( ref );
        }
        return value[ idx ];
    }

    PDFNull::PDFNull( ) : PDFBaseObject( PDFObjectType::Null )
    {
    }

    PDFNumber::PDFNumber( ) : PDFBaseObject( PDFObjectType::Number )
    {
    }

    PDFInteger::PDFInteger( std::int32_t v ) : value( v )
    {
    }

    auto PDFInteger::as_int( ) -> std::int32_t
    {
        return value;
    }

    auto PDFInteger::as_double( ) -> double
    {
        return static_cast<double>( value );
    }

    PDFReal::PDFReal( double v ) : value( v )
    {
    }

    auto PDFReal::as_int( ) -> std::int32_t
    {
        return static_cast<std::int32_t>( value );
    }

    auto PDFReal::as_double( ) -> double
    {
        return value;
    }

    PDFString::PDFString( ) : PDFBaseObject( PDFObjectType::String )
    {
    }

    auto PDFString::get_string( ) -> std::string
    {
        return "";
    }

    auto PDFString::get_data( ) -> data_t
    {
        return data_t( );
    }

    auto PDFString::set( in_data_t str ) -> void
    {
    }

    PDFByteString::PDFByteString( in_data_t d ) : data( d )
    {
    }

    auto PDFByteString::get_string( ) -> std::string
    {
        return data;
    }

    auto PDFByteString::get_data( ) -> data_t
    {
        return data;
    }

    auto PDFByteString::set( in_data_t str ) -> void
    {
        data = data_t( str );
    }

    PDFTextString::PDFTextString( std::string s ) : str( std::move( s ) )
    {
    }

    auto PDFTextString::get_string( ) -> std::string
    {
        return str;
    }

    auto PDFTextString::get_data( ) -> data_t
    {
        return str;
    }

    auto PDFTextString::set( in_data_t s ) -> void
    {
        str = s;
    }

    PDFUTF8String::PDFUTF8String( const std::string &s ) : PDFTextString( convert_from( s ) )
    {
    }

    auto PDFUTF8String::convert_from( const std::string &str ) -> std::string
    {
        // Check for UTF-8
        if ( str.length( ) >= 3 && str[ 0 ] == (char)239 && str[ 1 ] == (char)187 && str[ 2 ] == (char)191 )
            return str.substr( 3 );
        return str;
    }

    PDFDictionary::PDFDictionary(
        std::shared_ptr<PDFContext> ctx,
        std::unordered_map<std::shared_ptr<PDFName>, std::shared_ptr<PDFBaseObject>, PDFNamePtrHash, PDFNamePtrEqual>
            d )
        : PDFBaseObject( PDFObjectType::Dictionary ), context( std::move( ctx ) ), dict( std::move( d ) )
    {
    }

    auto PDFDictionary::get( const std::string &k, bool resolve ) -> std::shared_ptr<PDFBaseObject>
    {
        auto key = std::make_shared<PDFName>( k );
        if ( !dict.contains( key ) )
            return nullptr;
        if ( resolve && dict[ key ]->type == PDFObjectType::IndirectObject )
        {
            auto ref = dict[ key ]->to<PDFRef>( );
            return context->get_object( ref );
        }
        return dict[ key ];
    }

    auto PDFDictionary::set( const std::string &k, std::shared_ptr<PDFBaseObject> obj ) -> void
    {
        dict[ std::make_shared<PDFName>( k ) ] = std::move( obj );
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