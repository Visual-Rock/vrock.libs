module;

import vrock.utils.ByteArray;

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

export module vrock.pdf.PDFBaseObjects;

namespace vrock::pdf
{
    class PDFObjectParser;
    export class PDFContext;

    export enum class PDFObjectType
    {
        None,
        Bool,
        Integer,
        Real,
        String,
        Name,
        Array,
        Dictionary,
        Stream,
        Null,
        IndirectObject,
        Page,
        PageTreeNode,

        Operator, // used in Content stream parser

        Rectangle
    };

    export class PDFBaseObject : public std::enable_shared_from_this<PDFBaseObject>
    {
    public:
        explicit PDFBaseObject( PDFObjectType t ) : type( t )
        {
        }

        auto is( PDFObjectType t ) -> bool
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
            if ( this == nullptr )
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
                return false;
            return t == type;
        }

        template <class T>
            requires std::is_base_of_v<PDFBaseObject, T>
        std::shared_ptr<T> as( )
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
            if ( this == nullptr )
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
        }

        template <typename T, PDFObjectType ObjType = PDFObjectType::None>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto to( ) -> std::shared_ptr<T>
        {
            if ( ObjType == PDFObjectType::None )
                return as<T>( );
            if ( is( ObjType ) )
                return as<T>( );
            return nullptr;
        }

        PDFObjectType type = PDFObjectType::None;
    };

    export class PDFRef : public PDFBaseObject
    {
    public:
        PDFRef( uint32_t obj_num, uint32_t gen_num, uint8_t type )
            : PDFBaseObject( PDFObjectType::IndirectObject ), object_number( obj_num ), generation_number( gen_num ),
              type( type )
        {
            tag = std::format( "{} {} R", object_number, generation_number );
        }
        bool operator==( const PDFRef &rhs ) const
        {
            return object_number == rhs.object_number && generation_number == rhs.generation_number;
        }

        bool operator!=( const PDFRef &rhs ) const
        {
            return !( rhs == *this );
        }

        std::string tag;
        std::uint32_t object_number;
        std::uint32_t generation_number;
        std::uint8_t type;
    };

    struct PDFRefPtrHash
    {
    public:
        size_t operator( )( const std::shared_ptr<PDFRef> &r ) const
        {
            uint64_t l = ( (uint64_t)r->object_number ) << 32 | r->generation_number;
            return std::hash<uint64_t>( )( l );
        }
    };

    struct PDFRefPtrEqual
    {
        bool operator( )( const std::shared_ptr<PDFRef> &l, const std::shared_ptr<PDFRef> &r ) const
        {
            return l->object_number == r->object_number && l->generation_number == r->generation_number;
        }
    };

    static std::vector<std::pair<std::string, std::string>> irregular_characters = {
        { "#", "#23" },   { "\t", "#09" },  { "\n", "#0A" },  { "\f", "#0C" }, { "\r", "#0D" },  { " ", "#20" },
        { "\\(", "#28" }, { "\\)", "#29" }, { "<", "#3C" },   { ">", "#3C" },  { "\\[", "#5B" }, { "\\]", "#5D" },
        { "\\{", "#7B" }, { "\\}", "#7D" }, { "\\/", "#2F" }, { "%", "#25" }
    };

    export class PDFName : public PDFBaseObject
    {
    public:
        explicit PDFName( std::string n, bool parse = false ) : PDFBaseObject( PDFObjectType::Name )
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

        bool operator==( const PDFName &rhs ) const
        {
            return name == rhs.name;
        }
        bool operator!=( const PDFName &rhs ) const
        {
            return !( rhs == *this );
        }

        std::string name;
    };

    struct PDFNamePtrHash
    {
    public:
        size_t operator( )( const std::shared_ptr<PDFName> &n ) const
        {
            return std::hash<std::string>( )( n->name );
        }
    };

    struct PDFNamePtrEqual
    {
        bool operator( )( const std::shared_ptr<PDFName> &l, const std::shared_ptr<PDFName> &r ) const
        {
            return l->name == r->name;
        }
    };

    // Common PDF names
    export inline auto length = std::make_shared<PDFName>( "Length" );

    export class PDFBool : public PDFBaseObject
    {
    public:
        explicit PDFBool( bool b ) : PDFBaseObject( PDFObjectType::Bool ), value( b )
        {
        }

        bool value;
    };

    export class PDFArray : public PDFBaseObject
    {
    public:
        explicit PDFArray( std::vector<std::shared_ptr<PDFBaseObject>> v = { } )
            : PDFBaseObject( PDFObjectType::Array ), value( std::move( v ) )
        {
        }

        auto get( std::size_t idx ) -> std::shared_ptr<PDFBaseObject>
        {
            if ( value.size( ) < idx )
                return nullptr;
            return value[ idx ];
        }

        template <typename T, PDFObjectType ObjType = PDFObjectType::None>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get( std::size_t idx ) -> std::shared_ptr<T>
        {
            return get( idx )->to<T, ObjType>( );
        }

        std::vector<std::shared_ptr<PDFBaseObject>> value;
    };

    export class PDFNull : public PDFBaseObject
    {
    public:
        PDFNull( ) : PDFBaseObject( PDFObjectType::Null )
        {
        }
    };

    export class PDFInteger : public PDFBaseObject
    {
    public:
        explicit PDFInteger( std::int32_t v = 0 ) : PDFBaseObject( PDFObjectType::Integer ), value( v )
        {
        }

        std::int32_t value;
    };

    export class PDFReal : public PDFBaseObject
    {
    public:
        explicit PDFReal( double v = 0.0 ) : PDFBaseObject( PDFObjectType::Real ), value( v )
        {
        }

        double value;
    };

    export class PDFString : public PDFBaseObject
    {
    public:
        PDFString( ) : PDFBaseObject( PDFObjectType::String )
        {
        }

        virtual auto get_string( ) -> std::string
        {
        }

        virtual auto get_byte_array( ) -> std::shared_ptr<utils::ByteArray<>>
        {
        }

        virtual auto set( const std::string &str ) -> void
        {
        }

        virtual auto set( std::shared_ptr<utils::ByteArray<>> data ) -> void
        {
        }
    };

    export class PDFByteString : public PDFString
    {
    public:
        PDFByteString( std::shared_ptr<utils::ByteArray<>> d = nullptr ) : data( d )
        {
        }

        auto get_string( ) -> std::string override
        {
            return data->to_string( );
        }
        auto get_byte_array( ) -> std::shared_ptr<utils::ByteArray<>> override
        {
            return data;
        }
        auto set( const std::string &str ) -> void override
        {
            data = std::make_shared<utils::ByteArray<>>( str );
        }
        auto set( std::shared_ptr<utils::ByteArray<>> d ) -> void override
        {
            data = d;
        }

    private:
        std::shared_ptr<utils::ByteArray<>> data;
    };

    export class PDFTextString : public PDFString
    {
    public:
        explicit PDFTextString( std::string s ) : str( s )
        {
        }

        auto get_string( ) -> std::string override
        {
            return str;
        }
        auto get_byte_array( ) -> std::shared_ptr<utils::ByteArray<>> override
        {
            return std::make_shared<utils::ByteArray<>>( str );
        }
        auto set( const std::string &s ) -> void override
        {
            str = s;
        }
        auto set( std::shared_ptr<utils::ByteArray<>> data ) -> void override
        {
            str = data->to_string( );
        }

        // TODO: virtual auto convert_to() -> str::string = 0;
        virtual auto convert_from( const std::string &str ) -> std::string = 0;

    protected:
        std::string str;
    };

    export class PDFUTF8String : public PDFTextString
    {
    public:
        explicit PDFUTF8String( const std::string &s ) : PDFTextString( convert_from( s ) )
        {
        }

        auto convert_from( const std::string &str ) -> std::string final
        {
            if ( str.length( ) >= 3 && str[ 0 ] == (char)239 && str[ 1 ] == (char)187 && str[ 2 ] == (char)191 )
                return str.substr( 3 );
            return str;
        }
    };

    export class PDFDictionary : public PDFBaseObject
    {
    public:
        explicit PDFDictionary( std::shared_ptr<PDFContext> ctx = nullptr,
                                std::unordered_map<std::shared_ptr<PDFName>, std::shared_ptr<PDFBaseObject>,
                                                   PDFNamePtrHash, PDFNamePtrEqual>
                                    d = { } )
            : PDFBaseObject( PDFObjectType::Dictionary ), context( std::move( ctx ) ), dict( std::move( d ) )
        {
        }

        auto get( const std::string &k, bool resolve = true ) -> std::shared_ptr<PDFBaseObject>;

        template <typename T, PDFObjectType ObjType = PDFObjectType::None>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get( const std::string &k, bool resolve = true ) -> std::shared_ptr<T>
        {
            return get( k, resolve )->to<T, ObjType>( );
        }

        auto set( const std::string &k, std::shared_ptr<PDFBaseObject> obj ) -> void
        {
            dict[ std::make_shared<PDFName>( k ) ] = std::move( obj );
        }

        auto inline has( const std::string &k ) const -> bool
        {
            return dict.contains( std::make_shared<PDFName>( k ) );
        }

        std::unordered_map<std::shared_ptr<PDFName>, std::shared_ptr<PDFBaseObject>, PDFNamePtrHash, PDFNamePtrEqual>
            dict;

    private:
        std::shared_ptr<PDFContext> context;
    };

    export class XRefEntry
    {
    public:
        XRefEntry( ) = default;

        XRefEntry( std::uint32_t offset, std::uint32_t object_number, std::uint32_t gen_num, std::uint8_t type )
            : offset( offset ), object_number( object_number ), generation_number( gen_num ), type( type )
        {
        }

        std::uint32_t offset = 0;
        std::uint32_t object_number = 0;
        std::uint32_t generation_number = 0;
        std::uint8_t type = 0;
    };

    export class XRefTable
    {
    public:
        XRefTable( ) = default;

        auto get_entry( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<XRefEntry>
        {
            for ( auto e : entries )
                if ( e->type == 1 && e->object_number == ref->object_number )
                    return e; // NOLINT: to make the code clearer this is marked no lint
                else if ( e->type == 2 && e->generation_number == ref->object_number )
                    return e;
            return nullptr;
        }

        std::vector<std::shared_ptr<XRefEntry>> entries = { };
        std::shared_ptr<PDFDictionary> trailer;
    };

    export class BaseFilter
    {
    public:
        virtual auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        virtual auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;
    };

    export class PDFASCIIFilter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    export class PDFFlateFilter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    static std::unordered_map<std::string, std::shared_ptr<BaseFilter>> encodings = {
        { "ASCIIHexDecode", std::make_shared<PDFASCIIFilter>( ) },
        { "FlateDecode", std::make_shared<PDFFlateFilter>( ) },
        //    { "DCTDecode", std::make_shared<encodings::PDFDCTEncoding>( ) },
        //    { "JPXDecode", std::make_shared<encodings::PDFJPXEncoding>( ) },
    };

    export enum class PDFStreamType
    {
        Raw,
        XRef,
        Object
    };

    export class PDFStream : public PDFBaseObject
    {
    public:
        PDFStream( std::shared_ptr<PDFDictionary> dictionary, std::shared_ptr<utils::ByteArray<>> _data,
                   PDFStreamType t = PDFStreamType::Raw )
            : PDFBaseObject( PDFObjectType::Stream ), stream_type( t ), dict( std::move( dictionary ) )
        {
            // get filters and param dictionary
            auto filter = dict->get( "Filter" );
            auto p = dict->get( "DecodeParms" );
            auto param = std::make_shared<PDFDictionary>( );
            if ( p->is( PDFObjectType::Dictionary ) )
                param = p->as<PDFDictionary>( );
            std::vector<std::shared_ptr<PDFName>> filters{ };
            if ( filter->is( PDFObjectType::Name ) )
                filters.emplace_back( filter->as<PDFName>( ) );
            else if ( filter->is( PDFObjectType::Array ) )
            {
                auto arr = filter->as<PDFArray>( );
                for ( auto &item : arr->value )
                    if ( item->is( PDFObjectType::Name ) )
                        filters.emplace_back( item->as<PDFName>( ) );
            }

            // apply filters on data and save data
            auto d = std::move( _data );

            for ( auto &f : filters )
                if ( encodings.find( f->name ) != encodings.end( ) )
                    d = encodings[ f->name ]->decode( d, param );
            data = std::move( d );
        }

        template <typename T, PDFStreamType ObjType = PDFStreamType::Raw>
            requires std::is_base_of_v<PDFStream, T>
        auto to_stream( ) -> std::shared_ptr<T>
        {
            if ( ObjType == PDFStreamType::Raw )
                return as<T>( );
            if ( ObjType == stream_type )
                return as<T>( );
            return nullptr;
        }

        PDFStreamType stream_type;
        std::shared_ptr<PDFDictionary> dict;
        std::shared_ptr<utils::ByteArray<>> data;
    };

    export class PDFObjectStream : public PDFStream
    {
    public:
        explicit PDFObjectStream( std::shared_ptr<PDFDictionary> d, std::shared_ptr<utils::ByteArray<>> data )
            : PDFStream( std::move( d ), std::move( data ), PDFStreamType::Object )
        {
        }

    private:
        std::unordered_map<std::shared_ptr<PDFRef>, std::shared_ptr<PDFBaseObject>> objects = { };
        std::vector<std::pair<std::uint32_t, std::size_t>> offsets = { };
        std::size_t first = 0;
    };

    export class PDFXRefStream : public PDFStream
    {
    public:
        explicit PDFXRefStream( std::shared_ptr<PDFDictionary> d, std::shared_ptr<utils::ByteArray<>> data )
            : PDFStream( std::move( d ), std::move( data ), PDFStreamType::XRef )
        {
        }

        auto get_entries( ) -> std::vector<std::shared_ptr<XRefEntry>>;
    };

    export auto get_number( const std::shared_ptr<PDFBaseObject> &num ) -> double
    {
        if ( num->is( PDFObjectType::Integer ) )
            return num->as<PDFInteger>( )->value;
        else if ( num->is( PDFObjectType::Real ) )
            return num->as<PDFReal>( )->value;
        else
            return nan( "" );
    }

    class PDFContext
    {
    public:
        PDFContext( std::shared_ptr<PDFObjectParser> parser );

        auto get_object( const std::shared_ptr<PDFRef> & ) -> std::shared_ptr<PDFBaseObject>;

        auto init( ) -> void;

        template <typename T, PDFObjectType ObjType = PDFObjectType::None>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get_object( const std::shared_ptr<PDFRef> & ) -> std::shared_ptr<T>;

        std::shared_ptr<PDFObjectParser> parser;
        std::unordered_map<std::shared_ptr<PDFRef>, std::shared_ptr<PDFBaseObject>, PDFRefPtrHash, PDFRefPtrEqual>
            objects = { };

        std::shared_ptr<PDFDictionary> trailer;
        std::vector<std::shared_ptr<XRefTable>> xref_tables;
    };
} // namespace vrock::pdf