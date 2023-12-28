#include "vrock/pdf/parser/BaseParser.hpp"

namespace vrock::pdf
{
    BaseParser::BaseParser( std::string s, long o ) : _string( std::move( s ) ), _offset( o )
    {
    }
    
    auto BaseParser::parse_double( ) -> double
    {
        std::stringstream ss{ };
        while ( _offset < _string.length( ) )
        {
            if ( !is_digit_plus_minus_dot( _string[ _offset ] ) )
                break;
            ss.put( _string[ _offset ] );
            if ( _string[ _offset++ ] == '.' )
                break;
        }
        while ( _offset < _string.length( ) )
        {
            if ( !is_digit( _string[ _offset ] ) )
                break;
            ss.put( _string[ _offset++ ] );
        }
        return strtod( ss.str( ).c_str( ), nullptr );
    }

    auto BaseParser::parse_int( ) -> int
    {
        skip_comments_and_whitespaces( );
        std::stringstream ss{ };
        while ( _offset < _string.length( ) )
        {
            if ( !is_digit_plus_minus( _string[ _offset ] ) )
                break;
            ss.put( _string[ _offset++ ] );
        }
        if ( ss.str( ).length( ) != 0 )
            return std::stoi( ss.str( ) );
        return 0;
    }

    auto BaseParser::is_whitespace( char c ) -> bool
    {
        return std::find( _whitespace.begin( ), _whitespace.end( ), c ) != _whitespace.end( );
    }

    auto BaseParser::is_delimiter( char c ) -> bool
    {
        return std::find( _delimiter.begin( ), _delimiter.end( ), c ) != _delimiter.end( );
    }

    auto BaseParser::skip_whitespace( ) -> void
    {
        while ( _offset < _string.length( ) && is_whitespace( _string[ _offset ] ) )
            _offset++;
    }

    auto BaseParser::skip_comment( ) -> bool
    {
        if ( _string[ _offset ] != '%' )
            return false;
        while ( _offset < _string.length( ) )
        {
            if ( std::find( _endOfLine.begin( ), _endOfLine.end( ), _string[ _offset ] ) != _endOfLine.end( ) )
            {
                _offset++;
                return true;
            }
            _offset++;
        }
        return true;
    }

    auto BaseParser::skip_comments_and_whitespaces( ) -> void
    {
        skip_whitespace( );
        while ( skip_comment( ) )
            skip_whitespace( );
    }

    auto BaseParser::is_keyword( std::string key ) -> bool
    {
        for ( size_t i = _offset; i < _offset + key.length( ); i++ )
            if ( i >= _string.length( ) || _string[ i ] != key[ i - _offset ] )
                return false;
        _offset += key.length( );
        return true;
    }
} // namespace vrock::pdf