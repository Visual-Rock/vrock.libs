import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFObjectParser;

#include <gtest/gtest.h>

using namespace vrock::pdf;

TEST( ParsePDFName, BasicAssertions )
{
    {
        PDFObjectParser parser( "/Length sdfsdf /Test" );
        EXPECT_EQ( parser.parse_name( )->name, "Length" );
        EXPECT_ANY_THROW( parser.parse_name( ) );
        parser._offset = 15;
        EXPECT_EQ( parser.parse_name( )->name, "Test" );
    }
    {
        std::vector<std::string> results = { "Name1",
                                             "ASomewhatLongerName",
                                             "A;Name_With-Various***Characters?",
                                             "1.2",
                                             "$$",
                                             "@pattern",
                                             ".notdef",
                                             "Lime Green",
                                             "paired()parentheses",
                                             "The_Key_of_F#_Minor",
                                             "AB" };
        PDFObjectParser parser( "/Name1 /ASomewhatLongerName /A;Name_With-Various***Characters? /1.2 /$$ /@pattern "
                                "/.notdef /Lime#20Green /paired#28#29parentheses /The_Key_of_F#23_Minor/A#42" );
        for ( const auto &res : results )
        {
            EXPECT_EQ( parser.parse_name( )->name, res );
            parser.skip_whitespace( );
        }
    }
}

TEST( ParseHexString, BasicAssertions )
{
    PDFObjectParser parser( "<31323334>123" );
    EXPECT_EQ( parser.parse_hex_string( nullptr, false )->get_byte_array( )->to_string( ), "1234" );
    EXPECT_ANY_THROW( parser.parse_hex_string( nullptr, false ) );
}

TEST( ParseString, BasicAssertions )
{
    {
        PDFObjectParser parser( "(This is a string)\n"
                                "(Strings can contain newlines\n"
                                "and such.)\n"
                                "(Strings can contain balanced parentheses ()\n"
                                "and special characters ( * ! & } ^ %and so on) .)\n"
                                "(The following is an empty string .)\n"
                                "()\n"
                                "(It has zero (0) length.)" );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "This is a string" );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "Strings can contain newlines\nand such." );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ),
                   "Strings can contain balanced parentheses ()\nand special characters ( * ! & } ^ %and so on) ." );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "The following is an empty string ." );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "" );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "It has zero (0) length." );
        parser.skip_comments_and_whitespaces( );
        EXPECT_ANY_THROW( parser.parse_string( nullptr, false ) );
    }
    {
        PDFObjectParser parser( "(These \\\n"
                                "two strings \\\n"
                                "are the same.)\n"
                                "(These two strings are the same.)" );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "These two strings are the same." );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "These two strings are the same." );
    }
    {
        PDFObjectParser parser( "(This string has an end-of-line at the end of it.\n"
                                ")\n"
                                "(So does this one.\\n)" );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ),
                   "This string has an end-of-line at the end of it.\n" );
        parser.skip_comments_and_whitespaces( );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "So does this one.\n" );
    }
    {
        PDFObjectParser parser( "(\\053)(this is octal + sign \\53 so is this with a trailing 4 \\0534)" );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ), "+" );
        EXPECT_EQ( parser.parse_string( nullptr, false )->get_string( ),
                   "this is octal + sign + so is this with a trailing 4 +4" );
        parser.skip_comments_and_whitespaces( );
    }
}

TEST( ParseRefOrNumber, BasicAssertions )
{
    PDFObjectParser parser( "-13 37.25 555 12 0 R 11 0 obj" );
    auto num1 = parser.parse_number_or_reference( );
    EXPECT_EQ( num1->type, PDFObjectType::Integer );
    EXPECT_EQ( num1->as<PDFInteger>( )->value, -13 );

    parser.skip_comments_and_whitespaces( );
    auto num2 = parser.parse_number_or_reference( );
    EXPECT_EQ( num2->type, PDFObjectType::Real );
    EXPECT_EQ( num2->as<PDFReal>( )->value, 37.25 );

    parser.skip_comments_and_whitespaces( );
    auto num3 = parser.parse_number_or_reference( );
    EXPECT_EQ( num3->type, PDFObjectType::Integer );
    EXPECT_EQ( num3->as<PDFInteger>( )->value, 555 );

    parser.skip_comments_and_whitespaces( );
    auto ref = parser.parse_number_or_reference( );
    EXPECT_EQ( ref->type, PDFObjectType::IndirectObject );
    EXPECT_EQ( ref->as<PDFRef>( )->object_number, 12 );
    EXPECT_EQ( ref->as<PDFRef>( )->generation_number, 0 );

    parser.skip_comments_and_whitespaces( );
    auto ref1 = parser.parse_number_or_reference( );
    EXPECT_EQ( ref1->type, PDFObjectType::IndirectObject );
    EXPECT_EQ( ref1->as<PDFRef>( )->object_number, 11 );
    EXPECT_EQ( ref1->as<PDFRef>( )->generation_number, 0 );
}

TEST( ParseDictionaryOrStream, BasicAssertions )
{
    {
        PDFObjectParser parser( "<</Length 4>>stream\nTest\nendstream" );
        auto stream = parser.parse_dictionary_or_stream( nullptr, false );
        EXPECT_EQ( stream->type, PDFObjectType::Stream );
        EXPECT_EQ( stream->as<PDFStream>( )->data->to_string( ), "Test" );
    }
    {
        PDFObjectParser parser( "<<>>\nstream\nTest\nendstream" );
        auto stream = parser.parse_dictionary_or_stream( nullptr, false );
        EXPECT_EQ( stream->type, PDFObjectType::Stream );
        EXPECT_EQ( stream->as<PDFStream>( )->data->to_string( ), "Test" );
    }
    {
        PDFObjectParser parser( "<</Length 5/Test /Test /Active true>>" );
        auto dict = parser.parse_dictionary_or_stream( nullptr, false );
        EXPECT_EQ( dict->type, PDFObjectType::Dictionary );
        EXPECT_EQ( dict->as<PDFDictionary>( )->get<PDFInteger>( "Length" )->value, 5 );
        EXPECT_EQ( dict->as<PDFDictionary>( )->get<PDFName>( "Test" )->name, "Test" );
        EXPECT_EQ( dict->as<PDFDictionary>( )->get<PDFBool>( "Active" )->value, true );
    }
}

TEST( ParseArray, BasicAssertions )
{
    PDFObjectParser parser( "[<</Length 4>> /Name null false 47.3]" );
    auto arr = parser.parse_array( nullptr, false );
    EXPECT_EQ( arr->type, PDFObjectType::Array );
    EXPECT_EQ( arr->get<PDFDictionary>( 0 )->get<PDFInteger>( "Length" )->value, 4 );
    EXPECT_EQ( arr->get<PDFName>( 1 )->name, "Name" );
    EXPECT_EQ( arr->get( 2 )->type, PDFObjectType::Null );
    EXPECT_EQ( arr->get<PDFBool>( 3 )->value, false );
    EXPECT_EQ( arr->get<PDFReal>( 4 )->value, 47.3 );
}

TEST( ParseDictionary, BasicAssertions )
{
    PDFObjectParser parser( "<</Test true/Dict <</Nested true>>/Null null>>" );
    auto dict = parser.parse_dictionary( nullptr, false );
    EXPECT_EQ( dict->type, PDFObjectType::Dictionary );
    EXPECT_EQ( dict->get<PDFBool>( "Test" )->value, true );
    EXPECT_EQ( dict->get<PDFNull>( "Null" )->type, PDFObjectType::Null );
    EXPECT_EQ( dict->get( "Dict" )->type, PDFObjectType::Dictionary );
    EXPECT_EQ( dict->get<PDFDictionary>( "Dict" )->get<PDFBool>( "Nested" )->value, true );
}

TEST( ParseXref, BasicAssertions )
{
    PDFObjectParser parser( "12 0 obj\n"
                            "<</Type /XRef/Size 8/W [1 2 2]/Index [0 4 8 4]/Filter /ASCIIHexDecode/Length 103>>\n"
                            "stream\n"
                            "00 0000 FFFF\n"
                            "01 4724 0000\n"
                            "00 000D 0000\n"
                            "01 4B0C 0000\n"
                            "02 0001 0000\n"
                            "02 0001 0001\n"
                            "02 0001 0002\n"
                            "02 0001 0003\n"
                            "endstream\n"
                            "endobj\n"
                            "startxref\n"
                            "0\n"
                            "%%EOF\n"
                            "xref\n"
                            "0 6\n"
                            "0000000003 65535 f\n"
                            "0000000017 00000 n\n"
                            "0000000081 00000 n\n"
                            "0000000000 00007 f\n"
                            "0000000331 00000 n\n"
                            "0000000409 00000 n\n"
                            "trailer\n"
                            "<</Size 6/Prev 0>>\n"
                            "startxref\n"
                            "238\n"
                            "%%EOF" );
    auto context = std::make_shared<PDFContext>( std::shared_ptr<PDFObjectParser>( &parser ) );
    parser.set_context( context );
    auto xrefs = parser.parse_xref( );
    auto check_entry = []( const std::shared_ptr<XRefEntry> &e1, XRefEntry e2 ) {
        EXPECT_EQ( e1->offset, e2.offset );
        EXPECT_EQ( e1->object_number, e2.object_number );
        EXPECT_EQ( e1->generation_number, e2.generation_number );
        EXPECT_EQ( e1->type, e2.type );
    };
    EXPECT_EQ( xrefs.size( ), 2 );
    auto &xref1 = xrefs[ 0 ]; // normal text based
    auto &xref2 = xrefs[ 1 ]; // XRefStream
    EXPECT_EQ( xref1->entries.size( ), 6 );
    check_entry( xref1->entries[ 0 ], { 3, 0, 65535, 0 } );
    check_entry( xref1->entries[ 1 ], { 17, 1, 0, 1 } );
    check_entry( xref1->entries[ 2 ], { 81, 2, 0, 1 } );
    check_entry( xref1->entries[ 3 ], { 0, 3, 7, 0 } );
    check_entry( xref1->entries[ 4 ], { 331, 4, 0, 1 } );
    check_entry( xref1->entries[ 5 ], { 409, 5, 0, 1 } );
    EXPECT_EQ( xref2->entries.size( ), 8 );
    check_entry( xref2->entries[ 0 ], { 0, 0, 65535, 0 } );
    check_entry( xref2->entries[ 1 ], { 18212, 1, 0, 1 } );
    check_entry( xref2->entries[ 2 ], { 13, 2, 0, 0 } );
    check_entry( xref2->entries[ 3 ], { 19212, 3, 0, 1 } );
    check_entry( xref2->entries[ 4 ], { 0, 1, 8, 2 } );
    check_entry( xref2->entries[ 5 ], { 1, 1, 9, 2 } );
    check_entry( xref2->entries[ 6 ], { 2, 1, 10, 2 } );
    check_entry( xref2->entries[ 7 ], { 3, 1, 11, 2 } );
}