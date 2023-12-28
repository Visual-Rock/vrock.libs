#include <vrock/pdf/structure/PDFFilters.hpp>

#include <gtest/gtest.h>

using namespace vrock::pdf;
using namespace vrock::utils;

TEST( ASCIIHexFilter, BasicAssertions )
{
    auto filter = std::make_shared<PDFASCIIFilter>( );

    auto encoded = filter->encode( std::make_shared<ByteArray<>>( "Test" ), nullptr );
    EXPECT_EQ( encoded->to_string( ), "54657374>" );
    EXPECT_EQ( filter->decode( encoded, nullptr )->to_string( ), "Test" );
}