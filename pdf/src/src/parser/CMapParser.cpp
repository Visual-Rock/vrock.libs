#include "vrock/pdf/parser/CMapParser.hpp"

#include "vrock/pdf/parser/PDFObjectParser.hpp"
#include "vrock/pdf/structure/PDFEncryption.hpp"
#include "vrock/utils/NumberHelper.hpp"

#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>

namespace vrock::pdf
{
    auto to_utf8( const std::u16string &utf16 ) -> std::string
    {
        UErrorCode err = U_ZERO_ERROR;
        UConverter *conv = ucnv_open( "UTF-8", &err );
        auto bytes = ucnv_fromUChars( conv, nullptr, 0, utf16.data( ), utf16.size( ), &err );
        std::string result( bytes, 0 );
        err = U_ZERO_ERROR;
        auto ustr = icu::UnicodeString( utf16.data( ) );
        u_strToUTF8( result.data( ), bytes, &bytes, ustr.getBuffer( ), ustr.length( ), &err );
        return result;
    }

    class CMapParser : public PDFObjectParser
    {
    public:
        explicit CMapParser( const std::string &cmap ) : PDFObjectParser( cmap )
        {
            this->cmap = std::make_shared<UnicodeMap>( );
            _offset = _string.find( "begincmap" ) + 9;
            while ( !is_keyword( "endcmap" ) )
            {
                if ( is_keyword( "beginbfchar" ) )
                    parse_beginbfchar( );
                else if ( is_keyword( "beginbfrange" ) )
                    parse_beginbfrange( );
                else
                    parse_object( );
            }
            bool t;
        }

        auto parse_object( std::shared_ptr<PDFRef> ref = nullptr, bool decrypt = false )
            -> std::shared_ptr<PDFBaseObject>
        {
            skip_whitespace( );
            if ( is_keyword( "true" ) )
                return std::make_shared<PDFBool>( true );
            if ( is_keyword( "false" ) )
                return std::make_shared<PDFBool>( false );
            if ( is_keyword( "null" ) )
                return std::make_shared<PDFNull>( );

            if ( _string[ _offset ] == '<' && _string[ _offset + 1 ] == '<' )
                return parse_dictionary( ref, decrypt );
            if ( _string[ _offset ] == '/' )
                return parse_name( );
            if ( _string[ _offset ] == '<' )
                return parse_hex_string( ref, decrypt );
            if ( _string[ _offset ] == '(' )
                return parse_string( ref, decrypt );
            if ( _string[ _offset ] == '[' )
                return parse_array( ref, decrypt );
            if ( is_digit_plus_minus_dot( _string[ _offset ] ) )
                return parse_number_or_reference( );
            if ( _string[ _offset ] >= 'A' && _string[ _offset ] <= 'z' )
            {
                _offset++;
                while ( _string.length( ) > _offset &&
                        ( ( _string[ _offset + 1 ] >= 'A' && _string[ _offset + 1 ] <= 'Z' ) ||
                          ( _string[ _offset + 1 ] >= 'a' && _string[ _offset + 1 ] <= 'z' ) ||
                          _string[ _offset + 1 ] == '*' || _string[ _offset + 1 ] == '0' ||
                          _string[ _offset + 1 ] == '1' ) )
                    _offset++;
                return nullptr;
            }
            throw PDFParserException( std::format( "Unknown object: {}", _string.substr( _offset - 20, 50 ) ) );
        }

        auto parse_beginbfchar( ) -> void
        {
            while ( !is_keyword( "endbfchar" ) )
            {
                skip_comments_and_whitespaces( );
                auto src = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );
                auto dst = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );

                if ( src.size( ) > 4 )
                    throw PDFParserException( "hex string to large only 4 bytes supported" );

                std::uint32_t s = std::stoul( utils::to_hex_string( src ), nullptr, 16 );
                cmap->map[ s ] = to_utf8( utils::from_hex_string<std::u16string>( utils::to_hex_string( dst ) ) );
            }
            skip_comments_and_whitespaces( );
        }

        auto parse_beginbfrange( ) -> void
        {
            while ( !is_keyword( "endbfrange" ) )
            {
                skip_comments_and_whitespaces( );
                auto l = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );
                auto u = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );

                if ( l.size( ) > 4 || u.size( ) > 4 )
                    throw PDFParserException( "hex string to large only 4 bytes supported" );

                std::uint32_t lower = std::stoul( utils::to_hex_string( l ), nullptr, 16 );
                std::uint32_t upper = std::stoul( utils::to_hex_string( u ), nullptr, 16 );

                auto obj = parse_object( nullptr, false );
                if ( auto string = obj->to<PDFString>( ) )
                {
                    auto start = std::stoul( utils::to_hex_string( string->get_data( ) ), nullptr, 16 );
                    for ( std::uint32_t i = lower; i < upper + 1; i++ )
                        cmap->map[ i ] =
                            to_utf8( utils::from_hex_string<std::u16string>( utils::to_hex( start + ( i - lower ) ) ) );
                }
                else if ( auto arr = obj->to<PDFArray>( ) )
                {
                    for ( std::uint32_t i = lower; i < upper + 1; i++ )
                    {

                        if ( auto string = arr->get( i - lower )->to<PDFString>( ) )
                            cmap->map[ i ] = to_utf8(
                                utils::from_hex_string<std::u16string>( utils::to_hex_string( string->get_data( ) ) ) );
                    }
                }
                else
                {
                }
                skip_comments_and_whitespaces( );
            }
            skip_comments_and_whitespaces( );
        }

        std::shared_ptr<UnicodeMap> cmap;
    };

    auto parse_unicode_map( const std::string &cmap ) -> std::shared_ptr<UnicodeMap>
    {
        auto parser = CMapParser( cmap );
        parser.set_context( nullptr );
        parser.set_decryption_handler( std::make_shared<PDFNullSecurityHandler>( ) );
        return parser.cmap;
    }
} // namespace vrock::pdf