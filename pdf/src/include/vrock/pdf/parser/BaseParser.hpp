#pragma once

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace vrock::pdf
{
    class BaseParser
    {
    public:
        BaseParser( std::string s, long o ) : _string( std::move( s ) ), _offset( o )
        {
        }
        ~BaseParser( ) = default;

        auto parse_double( ) -> double
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

        auto parse_int( ) -> int
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

        auto inline is_whitespace( char c ) -> bool
        {
            return std::find( _whitespace.begin( ), _whitespace.end( ), c ) != _whitespace.end( );
        }

        auto is_delimiter( char c ) -> bool
        {
            return std::find( _delimiter.begin( ), _delimiter.end( ), c ) != _delimiter.end( );
        }

        auto inline is_digit( char c ) -> bool
        {
            return c >= '0' && c <= '9';
        }

        auto inline is_digit_plus_minus( char c ) -> bool
        {
            return ( c >= '0' && c <= '9' ) || c == '+' || c == '-';
        }

        auto inline is_digit_plus_minus_dot( char c ) -> bool
        {
            return ( c >= '0' && c <= '9' ) || c == '+' || c == '-' || c == '.';
        }

        auto skip_whitespace( ) -> void
        {
            while ( _offset < _string.length( ) && is_whitespace( _string[ _offset ] ) )
                _offset++;
        }

        auto skip_comment( ) -> bool
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

        auto skip_comments_and_whitespaces( ) -> void
        {
            skip_whitespace( );
            while ( skip_comment( ) )
                skip_whitespace( );
        }

        auto is_keyword( std::string key ) -> bool
        {
            for ( size_t i = _offset; i < _offset + key.length( ); i++ )
                if ( i >= _string.length( ) || _string[ i ] != key[ i - _offset ] )
                    return false;
            _offset += key.length( );
            return true;
        }

        auto advance( ) -> void
        {
            _offset++;
        }
        auto get_char( ) -> char
        {
            return _string[ _offset ];
        }

        std::string _string;
        size_t _offset;

    protected:
        const std::vector<char> _whitespace = { '\0', '\t', '\n', '\f', '\r', ' ' };
        const std::vector<char> _endOfLine = { '\n', /* '\r' */ };
        const std::vector<char> _delimiter = { '<', '>', '(', ')', '[', ']', '{', '}', '/', '%' };
    };
} // namespace vrock::pdf