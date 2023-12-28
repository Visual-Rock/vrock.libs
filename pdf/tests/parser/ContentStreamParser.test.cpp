#include "vrock/pdf/parser/ContentStreamParser.hpp"

#include <gtest/gtest.h>

using namespace vrock::pdf;

std::pair<std::shared_ptr<ResourceDictionary>, std::shared_ptr<PDFContext>> create_res_dict( )
{
    auto ctx = std::make_shared<PDFContext>( std::make_shared<PDFObjectParser>( ) );
    auto dict = std::make_shared<PDFDictionary>( ctx );
    auto res = std::make_shared<ResourceDictionary>( dict, ctx );
    return { res, ctx };
}

TEST( ParseOperatorCm, BasicAssertions )
{
    auto [ res, ctx ] = create_res_dict( );
    ContentStreamParser parser( "406.8 0 0 498.95 102.6 236.5 cm", res, ctx );
    auto mat = parser.graphic_state_stack.top( ).current_transformation_matrix;
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