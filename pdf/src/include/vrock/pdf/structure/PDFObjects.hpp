#pragma once

#include <vrock/utils/ByteArray.hpp>

#include <cstdint>
#include <format>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#if defined( __clang__ )
#define OPTNONE_START [[clang::optnone]]
#elif defined( __GNUC__ )
#define OPTNONE_START [[gnu::optimize( 0 )]]
#elif defined( _MSC_VER )
#define OPTNONE_START __pragma( optimize( "", off ) )
#else
#define OPTNONE_START
#endif

#if defined( _MSC_VER )
#define OPTNONE_END __pragma( optimize( "", on ) )
#else
#define OPTNONE_END
#endif

namespace vrock::pdf
{
    class PDFContext;
    enum class PDFObjectType
    {
        None,
        Bool,
        Number,
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

    template <typename T>
    auto to_object_type( ) -> PDFObjectType
    {
        static_assert( "unknown conversion from type" );
    }

    class PDFBaseObject : public std::enable_shared_from_this<PDFBaseObject>
    {
    public:
        explicit PDFBaseObject( PDFObjectType t );

        OPTNONE_START bool is( PDFObjectType t )
        {
            if ( this == nullptr )
                return false;
            return t == type;
            OPTNONE_END
        }

        template <class T>
            requires std::is_base_of_v<PDFBaseObject, T>
        OPTNONE_START std::shared_ptr<T> as( )
        {
            if ( this == nullptr )
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
            OPTNONE_END
        }

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto to( ) -> std::shared_ptr<T>
        {
            if ( is( to_object_type<T>( ) ) )
                return as<T>( );
            return nullptr;
        }

        const PDFObjectType type = PDFObjectType::None;
    };

    class PDFRef : public PDFBaseObject
    {
    public:
        PDFRef( uint32_t obj_num, uint32_t gen_num, uint8_t type );

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

    class PDFName : public PDFBaseObject
    {
    public:
        explicit PDFName( std::string n, bool parse = false );

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

    class PDFBool : public PDFBaseObject
    {
    public:
        explicit PDFBool( bool b ) : PDFBaseObject( PDFObjectType::Bool ), value( b )
        {
        }
        bool value;
    };

    class PDFArray : public PDFBaseObject
    {
    public:
        explicit PDFArray( std::shared_ptr<PDFContext> ctx, std::vector<std::shared_ptr<PDFBaseObject>> v = { } )
            : PDFBaseObject( PDFObjectType::Array ), value( std::move( v ) ), context( std::move( ctx ) )
        {
        }

        auto get( std::size_t idx, bool resolve = true ) -> std::shared_ptr<PDFBaseObject>;

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get( std::size_t idx, bool resolve = true ) -> std::shared_ptr<T>
        {
            return get( idx, resolve )->to<T>( );
        }

        std::vector<std::shared_ptr<PDFBaseObject>> value;

    private:
        std::shared_ptr<PDFContext> context;
    };

    class PDFNull : public PDFBaseObject
    {
    public:
        PDFNull( ) : PDFBaseObject( PDFObjectType::Null )
        {
        }
    };

    class PDFNumber : public PDFBaseObject
    {
    public:
        PDFNumber( ) : PDFBaseObject( PDFObjectType::Number )
        {
        }

        virtual auto as_int( ) -> std::int32_t = 0;
        virtual auto as_double( ) -> double = 0;
    };

    class PDFInteger : public PDFNumber
    {
    public:
        explicit PDFInteger( std::int32_t v = 0 ) : value( v )
        {
        }

        auto as_int( ) -> std::int32_t final
        {
            return value;
        }

        auto as_double( ) -> double final
        {
            return static_cast<double>( value );
        }

        std::int32_t value;
    };

    class PDFReal : public PDFNumber
    {
    public:
        explicit PDFReal( double v = 0.0 ) : value( v )
        {
        }

        auto as_int( ) -> std::int32_t final
        {
            return static_cast<std::int32_t>( value );
        }

        auto as_double( ) -> double final
        {
            return value;
        }

        double value;
    };

    class PDFString : public PDFBaseObject
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

    class PDFByteString : public PDFString
    {
    public:
        explicit PDFByteString( std::shared_ptr<utils::ByteArray<>> d = nullptr ) : data( std::move( d ) )
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

    class PDFTextString : public PDFString
    {
    public:
        explicit PDFTextString( std::string s ) : str( std::move( s ) )
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

    class PDFUTF8String : public PDFTextString
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

    class PDFDictionary : public PDFBaseObject
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

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get( const std::string &k, bool resolve = true ) -> std::shared_ptr<T>
        {
            if ( auto obj = get( k, resolve ) )
                return obj->to<T>( );
            return nullptr;
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

    template <>
    auto to_object_type<PDFNull>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFRef>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFBool>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFNumber>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFInteger>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFReal>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFString>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFName>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFArray>( ) -> PDFObjectType;
    template <>
    auto to_object_type<PDFDictionary>( ) -> PDFObjectType;
} // namespace vrock::pdf
