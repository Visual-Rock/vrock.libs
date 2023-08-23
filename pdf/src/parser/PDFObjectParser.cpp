module;

import vrock.utils.ByteArray;

#include <cmath>
#include <cstring>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

module vrock.pdf.PDFObjectParser;

import vrock.pdf.PDFBaseObjects;

namespace vrock::pdf
{
    PDFObjectParser::PDFObjectParser( ) : BaseParser( "", 0 )
    {
    }

    PDFObjectParser::PDFObjectParser( std::string s ) : BaseParser( std::move( s ), 0 )
    {
    }

    auto PDFObjectParser::parse_object( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFBaseObject>
    {
        skip_comments_and_whitespaces( );
        if ( is_keyword( "true" ) )
            return std::make_shared<PDFBool>( true );
        if ( is_keyword( "false" ) )
            return std::make_shared<PDFBool>( false );
        if ( is_keyword( "null" ) )
            return std::make_shared<PDFNull>( );

        if ( _string[ _offset ] == '<' && _string[ _offset + 1 ] == '<' )
            return parse_dictionary_or_stream( ref, decrypt );
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
        throw PDFParserException( std::format( "Unknown object: {}", _string.substr( _offset - 20, 50 ) ) );
    }

    auto PDFObjectParser::parse_dictionary( std::shared_ptr<PDFRef> ref, bool decrypt )
        -> std::shared_ptr<PDFDictionary>
    {
        _offset += 2;
        skip_comments_and_whitespaces( );
        auto dict = std::make_shared<PDFDictionary>( context );
        while ( _offset < _string.length( ) && _string[ _offset ] != '>' && _string[ _offset + 1 ] != '>' )
        {
            auto k = parse_name( );
            if ( k->name != "ID" )
                dict->dict[ k ] = parse_object( ref, decrypt );
            else
                dict->dict[ k ] = parse_object( ref, false );
            skip_comments_and_whitespaces( );
        }
        skip_comments_and_whitespaces( );
        _offset += 2;
        return dict;
    }

    auto PDFObjectParser::parse_name( ) -> std::shared_ptr<PDFName>
    {
        if ( _string[ _offset ] != '/' )
            throw PDFParserException( "Failed to parse name object. missing '/'." );
        _offset++;
        std::string str;
        while ( _offset < _string.length( ) )
        {
            if ( is_whitespace( _string[ _offset ] ) || is_delimiter( _string[ _offset ] ) )
                break;
            str += _string[ _offset++ ];
        }
        return std::make_shared<PDFName>( str, true );
    }

    auto PDFObjectParser::parse_hex_string( std::shared_ptr<PDFRef> ref, bool decrypt )
        -> std::shared_ptr<PDFByteString>
    {
        if ( _string[ _offset++ ] != '<' )
            throw PDFParserException( "Failed to Parse HEX String. String does not Start with '<'" );
        std::string str;
        while ( _string[ _offset ] != '>' )
        {
            if ( is_whitespace( _string[ _offset ] ) )
                skip_whitespace( );
            str += _string[ _offset++ ];
        }
        if ( str.length( ) % 2 == 1 )
            str += '0';
        ++_offset;

        if ( decrypt )
            return std::make_shared<PDFByteString>(
                decryption_handler->decrypt( utils::from_hex_string_shared( str ), std::move( ref ) ) );
        return std::make_shared<PDFByteString>( utils::from_hex_string_shared( str ) );
    }

    auto PDFObjectParser::parse_string( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFString>
    {
        int level = 0;
        bool escaped = false;
        std::string str;
        char c;

        while ( _offset < _string.length( ) )
        {
            c = _string[ _offset++ ];

            if ( escaped && c == '\n' )
                escaped = false;
            else if ( escaped && string_literal_lookup.find( c ) != string_literal_lookup.end( ) )
                str += string_literal_lookup.find( c )->second;
            else if ( escaped && is_digit( _string[ _offset ] ) ) // parse \ddd octal
            {
                std::string num;
                num += c;
                for ( int i = 0; i < 2; ++i )
                    if ( is_digit( _string[ _offset + i ] ) )
                        num += _string[ _offset + i ];
                    else
                        break;
                _offset += num.length( ) - 1; // first character got already incremented
                str += (char)std::strtoul( num.c_str( ), nullptr, 8 );
            }
            else if ( c != '\\' )
                str += c;

            if ( !escaped )
            {
                if ( c == '(' )
                    ++level;
                if ( c == ')' )
                    --level;
            }

            if ( c == '\\' && !escaped )
                escaped = true;
            else if ( escaped )
                escaped = false;

            if ( level == 0 )
            {
                if ( decrypt )
                    return std::make_shared<PDFUTF8String>(
                        decryption_handler
                            ->decrypt(
                                std::make_shared<vrock::utils::ByteArray<>>( str.substr( 1, str.length( ) - 2 ) ), ref )
                            ->to_string( ) );
                return std::make_shared<PDFUTF8String>( str.substr( 1, str.length( ) - 2 ) );
            }
        }
        throw PDFParserException( "Failed to Parse String Literal. Out of Bounds" );
    }

    auto PDFObjectParser::parse_array( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFArray>
    {
        if ( _string[ _offset++ ] != '[' )
            throw PDFParserException( "Array does not start with '['" );
        auto arr = std::make_shared<PDFArray>( context );
        while ( _offset < _string.length( ) && _string[ _offset ] != ']' )
        {
            arr->value.push_back( parse_object( ref, decrypt ) );
            skip_comments_and_whitespaces( );
        }
        ++_offset;
        return arr;
    }

    auto PDFObjectParser::parse_number_or_reference( ) -> std::shared_ptr<PDFBaseObject>
    {
        auto num = parse_double( );
        skip_comments_and_whitespaces( );
        auto tmp = _offset;
        if ( is_digit( _string[ _offset ] ) )
        {
            auto num2 = parse_double( );
            skip_comments_and_whitespaces( );
            if ( is_keyword( "obj" ) || _string[ _offset++ ] == 'R' )
                return std::make_shared<PDFRef>( (int)num, (int)num2, 1 );
        }
        _offset = tmp;

        double temp;
        if ( modf( num, &temp ) != 0 )
            return std::make_shared<PDFReal>( num );
        return std::make_shared<PDFInteger>( (int)num );
    }

    auto PDFObjectParser::parse_dictionary_or_stream( std::shared_ptr<PDFRef> ref, bool decrypt )
        -> std::shared_ptr<PDFBaseObject>
    {
        auto dict = parse_dictionary( ref, decrypt );
        skip_comments_and_whitespaces( );
        if ( !is_keyword( "stream \r\n" ) && !is_keyword( "stream\r\n" ) && !is_keyword( "stream\n" ) &&
             !is_keyword( "stream" ) )
            return dict;
        auto start = _offset;
        size_t end = 0;
        if ( auto len = dict->get<PDFInteger>( "Length" ) )
        {
            end = start + len->value;
            _offset = end;
            skip_comments_and_whitespaces( );
            if ( !is_keyword( "endstream" ) )
            {
                _offset = start;
                end = find_end_of_stream( );
            }
        }
        else
            end = find_end_of_stream( );
        auto data = std::make_shared<utils::ByteArray<>>( end - start );
        std::memcpy( data->data( ), _string.data( ) + start, data->size( ) );

        auto type = dict->get<PDFName>( "Type" );
        if ( type && type->name == "XRef" )
            return std::make_shared<PDFXRefStream>( dict, data );
        if ( type && type->name == "ObjStm" )
            return std::make_shared<PDFObjectStream>( dict, decrypt ? decryption_handler->decrypt( data, ref ) : data,
                                                      context );
        return std::make_shared<PDFStream>( dict, decrypt ? decryption_handler->decrypt( data, ref ) : data );
    }

    auto PDFObjectParser::find_end_of_stream( ) -> std::size_t
    {
        auto level = 1;
        auto end = _offset;
        while ( _offset < _string.length( ) )
        {
            end = _offset;
            if ( is_keyword( "stream" ) )
                level++;
            else if ( is_keyword( "\r\nendstream" ) || is_keyword( "\rendstream" ) || is_keyword( "\nendstream" ) ||
                      is_keyword( "endstream" ) )
                level--;
            else
                _offset++;
            if ( level == 0 )
                break;
        }
        if ( level != 0 )
            throw PDFParserException( "Failed to Parse Stream reached end of file" );
        return end;
    }

    auto PDFObjectParser::parse_xref( std::size_t offset ) -> std::vector<std::shared_ptr<XRefTable>>
    {
        std::size_t new_offset = offset;
        std::vector<std::shared_ptr<XRefTable>> tables = { };
        tables.emplace_back( std::make_shared<XRefTable>( ) );
        auto &table = tables[ 0 ];

        if ( offset == -1 )
        {
            auto start = _string.find_last_of( "startxref" );
            _offset = ++start;
            skip_comments_and_whitespaces( );
            new_offset = parse_int( );
            _offset = new_offset;
        }

        if ( is_keyword( "xref" ) ) // Text-based XRef table
        {
            skip_comments_and_whitespaces( );
            std::shared_ptr<XRefEntry> entry;

            while ( !is_keyword( "trailer" ) )
            {
                auto start = parse_int( );
                skip_whitespace( );
                auto amount = parse_int( );
                skip_whitespace( );

                table->entries.reserve( table->entries.size( ) + amount );

                for ( int i = 0; i < amount; ++i )
                {
                    entry = std::make_shared<XRefEntry>( );
                    entry->offset = parse_int( );
                    skip_whitespace( );
                    entry->generation_number = parse_int( );
                    skip_whitespace( );
                    entry->object_number = start + i;
                    // 0: free object 1: used object correspond to f for free and n for used
                    entry->type = get_char( ) == 'n';
                    _offset++;
                    table->entries.push_back( entry );
                    skip_whitespace( );
                }

                skip_whitespace( );
            }
            skip_comments_and_whitespaces( );
            table->trailer = parse_dictionary( nullptr, false );
        }
        else
        {
            auto ref = parse_object( nullptr, false )->to<PDFRef>( );
            auto obj = parse_object( nullptr, false );
            if ( auto stream = obj->to<PDFStream>( )->to_stream<PDFXRefStream, PDFStreamType::XRef>( ) )
            {
                table->entries = std::move( stream->get_entries( ) );
                table->trailer = stream->dict;
            }
            else if ( auto dict = obj->to<PDFDictionary>( ) )
                throw PDFParserException( "Linearized PDF's are not supported" );
            else
                throw PDFParserException( "Not a Dictionary or XRefStream" );

            context->objects[ ref ] = obj;
        }

        if ( table->trailer && table->trailer->has( "Prev" ) ) // there are more XRefTables to parse
        {
            if ( auto prev = table->trailer->get<PDFInteger>( "Prev", false ) )
            {
                if ( prev->value == new_offset )
                    return tables;
                _offset = prev->value;
                auto prev_tables = parse_xref( prev->value );
                tables.reserve( prev_tables.size( ) + 1 );
                for ( auto &t : prev_tables )
                    tables.push_back( t );
            }
        }
        return tables;
    }
} // namespace vrock::pdf