#include "vrock/pdf/parser/ContentStreamParser.hpp"

#include "Helper.hpp"

#include <gtest/gtest.h>

using namespace vrock::pdf;

TEST( ParseText, ParseOperatorTst )
{
    auto [ res, ctx ] = create_res_dict( );
    ContentStreamParser parser( "40 50 TD T*", res, ctx );
    auto mat = parser.graphic_state_stack.top( ).text_state.current_text_matrix;
    EXPECT_EQ( mat[ 0 ][ 0 ], 1.0 );
    EXPECT_EQ( mat[ 0 ][ 1 ], 0.0 );
    EXPECT_EQ( mat[ 0 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 1 ][ 0 ], 0.0 );
    EXPECT_EQ( mat[ 1 ][ 1 ], 1.0 );
    EXPECT_EQ( mat[ 1 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 2 ][ 0 ], 40.0 );
    EXPECT_EQ( mat[ 2 ][ 1 ], 100 );
    EXPECT_EQ( mat[ 2 ][ 2 ], 1.0 );

    EXPECT_EQ( parser.graphic_state_stack.top( ).text_state.leading, -50 );
}

TEST( ParseText, ParseOperatorTd )
{
    auto [ res, ctx ] = create_res_dict( );
    ContentStreamParser parser( "40 50 Td", res, ctx );
    auto mat = parser.graphic_state_stack.top( ).text_state.current_text_matrix;
    EXPECT_EQ( mat[ 0 ][ 0 ], 1.0 );
    EXPECT_EQ( mat[ 0 ][ 1 ], 0.0 );
    EXPECT_EQ( mat[ 0 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 1 ][ 0 ], 0.0 );
    EXPECT_EQ( mat[ 1 ][ 1 ], 1.0 );
    EXPECT_EQ( mat[ 1 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 2 ][ 0 ], 40.0 );
    EXPECT_EQ( mat[ 2 ][ 1 ], 50.0 );
    EXPECT_EQ( mat[ 2 ][ 2 ], 1.0 );
}

TEST( ParseText, ParseOperatorTD )
{
    auto [ res, ctx ] = create_res_dict( );
    ContentStreamParser parser( "40 50 TD", res, ctx );
    auto mat = parser.graphic_state_stack.top( ).text_state.current_text_matrix;
    EXPECT_EQ( mat[ 0 ][ 0 ], 1.0 );
    EXPECT_EQ( mat[ 0 ][ 1 ], 0.0 );
    EXPECT_EQ( mat[ 0 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 1 ][ 0 ], 0.0 );
    EXPECT_EQ( mat[ 1 ][ 1 ], 1.0 );
    EXPECT_EQ( mat[ 1 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 2 ][ 0 ], 40.0 );
    EXPECT_EQ( mat[ 2 ][ 1 ], 50.0 );
    EXPECT_EQ( mat[ 2 ][ 2 ], 1.0 );

    EXPECT_EQ( parser.graphic_state_stack.top( ).text_state.leading, -50 );
}

TEST( ParseText, ParseOperatorTm )
{
    auto [ res, ctx ] = create_res_dict( );
    ContentStreamParser parser( "406.8 0 0 498.95 102.6 236.5 Tm", res, ctx );
    auto mat = parser.graphic_state_stack.top( ).text_state.current_text_matrix;
    EXPECT_EQ( mat[ 0 ][ 0 ], 406.8 );
    EXPECT_EQ( mat[ 0 ][ 1 ], 0.0 );
    EXPECT_EQ( mat[ 0 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 1 ][ 0 ], 0.0 );
    EXPECT_EQ( mat[ 1 ][ 1 ], 498.95 );
    EXPECT_EQ( mat[ 1 ][ 2 ], 0.0 );

    EXPECT_EQ( mat[ 2 ][ 0 ], 102.6 );
    EXPECT_EQ( mat[ 2 ][ 1 ], 236.5 );
    EXPECT_EQ( mat[ 2 ][ 2 ], 1.0 );
}

TEST( ParseText, ParseOperatorTfTdTj )
{
    auto [ res, ctx ] = create_res_dict( );
    res->fonts[ "F0" ] = create_font( ctx );
    ContentStreamParser parser( "BT /F0 12 Tf 288 720 Td (ABC) Tj ET", res, ctx );

    auto text = parser.text;
    EXPECT_EQ( text.size( ), 1 );

    auto t = text[ 0 ];
    EXPECT_EQ( t->strings.size( ), 1 );

    auto str = t->strings[ 0 ];
    EXPECT_EQ( str->font_size, 12 );
    EXPECT_EQ( str->font, res->fonts[ "F0" ] );
    EXPECT_EQ( str->position.x.units, 288.0 );
    EXPECT_EQ( str->position.y.units, 720.0 );
    EXPECT_EQ( str->text, "ABC" );
}