import vrock.pdf.PDFDocument;

#include <gtest/gtest.h>

#include <iostream>

using namespace vrock::pdf;

TEST( ExternalImage, BasicAssertions )
{
    auto doc = PDFDocument( "pdfs/image.pdf" );
    auto img = doc.get_page( 0 )->get_images( );
    EXPECT_EQ( img.size( ), 1 );
    EXPECT_EQ( img[ 0 ]->position.x.units, 102.6f );
    EXPECT_EQ( img[ 0 ]->position.y.units, 236.5f );
    EXPECT_EQ( img[ 0 ]->scale.x.units, 406.8f );
    EXPECT_EQ( img[ 0 ]->scale.y.units, 498.95f );
    EXPECT_EQ( img[ 0 ]->rotation, 0.0 );
    EXPECT_EQ( img[ 0 ]->shear, 0.0 );
    // TODO: Add test for rotated and shear image
}