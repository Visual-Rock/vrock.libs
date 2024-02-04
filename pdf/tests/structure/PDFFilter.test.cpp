#include <vrock/pdf/structure/PDFFilters.hpp>

#include <gtest/gtest.h>

using namespace vrock::pdf;

TEST( ASCIIHexFilter, BasicAssertions )
{
    auto filter = std::make_shared<PDFASCIIFilter>( );

    auto encoded = filter->encode( "Test", nullptr );
    EXPECT_EQ( encoded, "54657374>" );
    EXPECT_EQ( filter->decode( encoded, nullptr ), "Test" );
}