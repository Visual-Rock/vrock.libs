#include "vrock/pdf/parser/CMapParser.hpp"

#include "vrock/pdf/structure/PDFEncryption.hpp"
#include "vrock/pdf/parser/PDFObjectParser.hpp"

namespace vrock::pdf
{
    class CMapParser : public PDFObjectParser
    {
    public:
        explicit CMapParser( const std::string &cmap ) : PDFObjectParser( cmap )
        {
            this->cmap = std::make_shared<CMap>( );
            _offset = _string.find( "begincmap" ) + 9;
            while ( !is_keyword( "endcmap" ) )
            {
                if ( is_keyword( "beginbfchar" ) )
                {
                    parse_beginbfchar( );
                }
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
            skip_whitespace( );

            while ( !is_keyword( "endbfchar" ) )
            {
                auto src = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );
                auto dst = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );

                if ( src.size( ) > 4 || dst.size( ) > 4 )
                    throw PDFParserException( "hex string to large only 4 bytes supported" );

                std::uint32_t s = 0, d = 0;
                s = std::stoul( utils::to_hex_string( src ), nullptr, 16 );
                d = std::stoul( utils::to_hex_string( dst ), nullptr, 16 );
                cmap->map[ s ] = d;
            }
            skip_comments_and_whitespaces( );
        }

        auto parse_beginbfrange() -> void
        {
            while (!is_keyword("beginbfrange"))
            {
                auto l = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );
                auto u = parse_hex_string( nullptr, false )->get_data( );
                skip_comments_and_whitespaces( );

                if ( l.size( ) > 4 || u.size( ) > 4 )
                    throw PDFParserException( "hex string to large only 4 bytes supported" );

                std::uint32_t lower = std::stoul( utils::to_hex_string( l ), nullptr, 16 );
                std::uint32_t upper = std::stoul( utils::to_hex_string( u ), nullptr, 16 );

                auto obj = parse_object( nullptr, false );
                if (auto string = obj->to<PDFString>())
                {
                    std::uint32_t start = std::stoul( utils::to_hex_string( string->get_data( ) ), nullptr, 16 );
                    for ( std::uint32_t i = lower; i < upper + 1 ; i++ )
                        cmap->map[ i ] = start + ( c - lower );
                }
                else if (auto arr = obj->to<PDFArray>())
                {
                }
                else
                {
                }
            }
        }

        std::shared_ptr<CMap> cmap;
    };

    auto parse_cmap( const std::string &cmap ) -> std::shared_ptr<CMap>
    {
        auto parser = CMapParser( cmap );
        parser.set_context( nullptr );
        parser.set_decryption_handler( std::make_shared<PDFNullSecurityHandler>( ) );
        return parser.cmap;
    }
} // namespace vrock::pdf