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
        BaseParser( std::string s, long o );
        ~BaseParser( ) = default;

        auto parse_double( ) -> double;

        auto parse_int( ) -> int;

        auto is_whitespace( char c ) -> bool;

        auto is_delimiter( char c ) -> bool;

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

        auto skip_whitespace( ) -> void;

        auto skip_comment( ) -> bool;

        auto skip_comments_and_whitespaces( ) -> void;

        auto is_keyword( std::string key ) -> bool;

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