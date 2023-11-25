#include "vrock/pdf/structure/Functions.hpp"

#include <gtest/gtest.h>

using namespace vrock::pdf;

TEST( FunctionConstructor, BasicAssertions )
{
    auto dict = std::make_shared<PDFDictionary>( );
    auto arr = std::make_shared<PDFArray>( nullptr );
    arr->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    arr->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    arr->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    arr->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    dict->set( "Domain", arr );

    auto fn = std::make_shared<Function>( dict );
}