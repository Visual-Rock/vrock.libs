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

TEST( ParseText, ParseOperatorTf )
{
    auto [ res, ctx ] = create_res_dict( );
    res->fonts[ "F0" ] = create_font( ctx );
    ContentStreamParser parser( "/F0 12 Tf", res, ctx );
    auto state = parser.graphic_state_stack.top( ).text_state;
    EXPECT_EQ( state.font, res->fonts[ "F0" ] );
    EXPECT_EQ( state.font_size, 12 );
}

TEST( ParseText, ParseOperatorTj )
{
    auto [ res, ctx ] = create_res_dict( );
    res->fonts[ "F0" ] = create_font( ctx );
    ContentStreamParser parser( "BT /F0 12 Tf (ABC) Tj ET", res, ctx );
    auto text = parser.text;
    EXPECT_EQ( text.size(), 1 );
    auto str = text[ 0 ];
    EXPECT_EQ( str->strings.size(), 1);
    EXPECT_EQ( str->strings[ 0 ]->text, "ABC" );
    EXPECT_EQ( str->strings[ 0 ]->font, res->fonts[ "F0" ] );
    EXPECT_EQ( str->strings[ 0 ]->font_size, 12 );
    EXPECT_EQ( str->strings[ 0 ]->offsets.size( ), 1 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[0].first, 3 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[0].second, 0 );
}

TEST( ParseText, ParseOperatorTJ )
{
    auto [ res, ctx ] = create_res_dict( );
    res->fonts[ "F0" ] = create_font( ctx );
    ContentStreamParser parser( "BT /F0 12 Tf [(A) 120 (W) 120 (A) 95 (Y again) ] TJ ET", res, ctx );
    auto text = parser.text;
    EXPECT_EQ( text.size( ), 1 );
    auto str = text[ 0 ];
    EXPECT_EQ( str->strings.size( ), 1 );
    EXPECT_EQ( str->strings[ 0 ]->text, "AWAY again" );
    EXPECT_EQ( str->strings[ 0 ]->font, res->fonts[ "F0" ] );
    EXPECT_EQ( str->strings[ 0 ]->font_size, 12 );
    EXPECT_EQ( str->strings[ 0 ]->offsets.size( ), 4 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 0 ].first, 1 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 0 ].second, 0 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 1 ].first, 1 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 1 ].second, 120 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 2 ].first, 1 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 2 ].second, 120 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 3 ].first, 7 );
    EXPECT_EQ( str->strings[ 0 ]->offsets[ 3 ].second, 95 );
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