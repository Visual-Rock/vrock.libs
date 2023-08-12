module;

#include <iostream>
#include <utility>
#include <vector>

export module vrock.pdf.BaseParser;

namespace vrock::pdf
{
    export class ParserException : public std::exception
    {
    public:
        explicit ParserException( std::string m ) : message( std::move( m ) )
        {
        }

        std::string what( )
        {
            return message;
        }

    private:
        std::string message;
    };

    export class BaseParser
    {
    public:
        BaseParser( std::string s, long o );
        ~BaseParser( );

        auto parse_double( ) -> double;
        auto parse_int( ) -> int;

        auto is_whitespace( char c ) -> bool;
        auto is_delimiter( char c ) -> bool;
        static auto is_digit( char c ) -> bool;
        static auto is_digit_plus_minus( char c ) -> bool;
        static auto is_digit_plus_minus_dot( char c ) -> bool;

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