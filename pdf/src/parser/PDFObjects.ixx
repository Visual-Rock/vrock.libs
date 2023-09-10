module;

import vrock.utils.ByteArray;
import vrock.utils.List;
import vrock.pdf.PDFDataStructures;

#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <format>
#include <iomanip>
#include <iostream>
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

    export template <typename T>
    auto to_object_type( ) -> PDFObjectType
    {
        static_assert( "unknown conversion from type" );
    }

    export class PDFBaseObject : public std::enable_shared_from_this<PDFBaseObject>
    {
    public:
        explicit PDFBaseObject( PDFObjectType t ) : type( t )
        {
        }

        bool is( PDFObjectType t ) __attribute__( ( optnone ) )
        {
            if ( this == nullptr )
                return false;
            return t == type;
        }

        template <class T>
            requires std::is_base_of_v<PDFBaseObject, T>
        std::shared_ptr<T> as( ) __attribute__( ( optnone ) )
        {
            if ( this == nullptr )
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
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

    export class PDFDCTFilter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    export class PDFBase85Filter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    static std::unordered_map<std::string, std::shared_ptr<BaseFilter>> encodings = {
        { "ASCIIHexDecode", std::make_shared<PDFASCIIFilter>( ) },
        { "ASCII85Decode", std::make_shared<PDFBase85Filter>( ) },
        { "FlateDecode", std::make_shared<PDFFlateFilter>( ) },
        // TODO: implement JPXDecode and DCTDecode correctly
        { "DCTDecode", std::make_shared<PDFDCTFilter>( ) },
        { "JPXDecode", std::make_shared<PDFDCTFilter>( ) },
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
            if ( auto name = filter->to<PDFName>( ) )
                filters.emplace_back( name->name );
            else if ( auto arr = filter->to<PDFArray>( ) )
            {
                for ( auto &item : arr->value )
                    if ( auto name = item->to<PDFName>( ) )
                        filters.emplace_back( name->name );
            }

            // apply filters on data and save data
            auto d = std::move( _data );

            for ( auto &f : filters )
                if ( encodings.contains( f ) )
                    d = encodings[ f ]->decode( d, param );
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

        utils::List<std::string> filters = { };
    };

    export class PDFObjectStream : public PDFStream
    {
    public:
        explicit PDFObjectStream( std::shared_ptr<PDFDictionary> d, std::shared_ptr<utils::ByteArray<>> data,
                                  std::shared_ptr<PDFContext> ctx );

        auto get_object( std::size_t idx ) -> std::shared_ptr<PDFBaseObject>;

    private:
        std::unordered_map<std::shared_ptr<PDFRef>, std::shared_ptr<PDFBaseObject>> objects = { };
        std::vector<std::pair<std::uint32_t, std::size_t>> offsets = { };
        std::shared_ptr<PDFObjectParser> parser;
        std::shared_ptr<PDFContext> context;
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

    // Pages

    export class Rectangle : public PDFBaseObject
    {
    public:
        explicit Rectangle( std::shared_ptr<PDFArray> arr );

        Point lower_left;
        Point upper_left;
        Point upper_right;
        Point lower_right;

        auto get_width( ) const -> Unit;
        auto get_height( ) const -> Unit;
    };

    export enum class FontType
    {
        None,
        Type1,
        TrueType
    };

    export class Font
    {
    public:
        Font( std::shared_ptr<PDFDictionary> dict );
        ~Font( ) = default;

        /// Subtype
        FontType font_type;

        std::string base_font;
        std::string font_name;

        std::int32_t first_char = 0;
        std::int32_t last_char = 0;

        std::vector<int32_t> widths = { };

        // std::shared_ptr<FontDescriptor> descriptor = nullptr;

        // TODO: Encoding

        // std::shared_ptr<structure::ToUnicode> to_unicode;
    };

    export enum class ColorSpaceType
    {
        RGB,
        Gray,
        CMYK,
        Indexed
    };

    export class ColorSpace
    {
    public:
        explicit ColorSpace( ColorSpaceType t ) : type( t )
        {
        }

        virtual auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        ColorSpaceType type;
    };

    export class RGBColorSpace : public ColorSpace
    {
    public:
        RGBColorSpace( ) : ColorSpace( ColorSpaceType::RGB )
        {
        }

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> final
        {
            return data;
        }
    };

    /**
     * @param stride in bit
     * */
    auto get_num( std::uint8_t *data, std::size_t idx, std::uint8_t stride ) -> std::uint8_t
    {
        auto bit_offset = idx * stride;
        auto byte_offset = bit_offset / 8;
        auto offset = 7 - ( bit_offset % 8 );
        auto mask = ( ( (uint8_t)std::pow( 2, stride ) ) - 1 ) << offset;
        return ( data[ byte_offset ] & mask ) >> offset;
    }

    export class GrayColorSpace : public ColorSpace
    {
    public:
        explicit GrayColorSpace( ) : ColorSpace( ColorSpaceType::Gray )
        {
        }

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> params )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    export class CMYKColorSpace : public ColorSpace
    {
    public:
        explicit CMYKColorSpace( ) : ColorSpace( ColorSpaceType::CMYK )
        {
        }

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> final
        {
            return data;
        }
    };

    export class IndexedColorSpace : public ColorSpace
    {
    public:
        explicit IndexedColorSpace( std::shared_ptr<PDFArray> );

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>>;

    protected:
        std::int32_t highest_value = 0;
        std::shared_ptr<utils::ByteArray<>> map = nullptr;
    };

    export auto to_colorspace( std::shared_ptr<PDFBaseObject> ) -> std::shared_ptr<ColorSpace>;

    export enum class ImageSaveFormat
    {
        png
    };

    export class PDFImage
    {
    public:
        PDFImage( std::shared_ptr<PDFStream> stm );

        auto save( const std::string &path, ImageSaveFormat format = ImageSaveFormat::png ) -> void;

        inline auto get_width( ) const -> std::int32_t
        {
            return width;
        }

        inline auto get_height( ) const -> std::int32_t
        {
            return height;
        }

        inline auto get_bits_per_pixel( ) const -> std::int32_t
        {
            return bpp;
        }

        inline auto as_rgb( ) -> std::shared_ptr<utils::ByteArray<>>
        {
            return color_space->convert_to_rgb( stream->data, stream->dict );
        }

        auto as_rgba( ) -> std::shared_ptr<utils::ByteArray<>>
        {
            auto rgb = as_rgb( );
            auto converted = std::make_shared<utils::ByteArray<>>( ( rgb->size( ) / 3 ) * 4 );
            for ( auto i = 0; i < rgb->size( ) / 3; i++ )
            {
                std::memcpy( converted->data( ) + ( 4 * i ), rgb->data( ) + ( 3 * i ), 3 );
                converted->at( ( 4 * i ) + 3 ) = 0xff;
            }
            // TODO: Image mask Soft Masks...
            return converted;
        }

    private:
        std::int32_t width = 0, height = 0, channel = 3;
        std::uint8_t bpp = 8;
        std::shared_ptr<PDFStream> stream;
        std::shared_ptr<ColorSpace> color_space;
    };

    class PDFContext
    {
    public:
        PDFContext( std::shared_ptr<PDFObjectParser> parser );

        auto get_object( const std::shared_ptr<PDFRef> & ) -> std::shared_ptr<PDFBaseObject>;

        auto init( ) -> void;

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get_object( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<T>
        {
            return get_object( ref )->to<T>( );
        }

        std::shared_ptr<PDFObjectParser> parser;
        std::unordered_map<std::shared_ptr<PDFRef>, std::shared_ptr<PDFBaseObject>, PDFRefPtrHash, PDFRefPtrEqual>
            objects = { };

        std::shared_ptr<PDFDictionary> trailer;
        std::vector<std::shared_ptr<XRefTable>> xref_tables;
    };

    template <>
    auto to_object_type<PDFBool>( ) -> PDFObjectType
    {
        return PDFObjectType::Bool;
    }

    template <>
    auto to_object_type<PDFInteger>( ) -> PDFObjectType
    {
        return PDFObjectType::Integer;
    }

    template <>
    auto to_object_type<PDFReal>( ) -> PDFObjectType
    {
        return PDFObjectType::Real;
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

    template <>
    auto to_object_type<PDFStream>( ) -> PDFObjectType
    {
        return PDFObjectType::Stream;
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

    class PDFOperator;
    template <>
    auto to_object_type<PDFOperator>( ) -> PDFObjectType
    {
        return PDFObjectType::Operator;
    }
} // namespace vrock::pdf