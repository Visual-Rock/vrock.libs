#include "vrock/pdf/structure/Functions.hpp"

#include <gtest/gtest.h>

using namespace vrock::pdf;

TEST( FunctionConstructor, BasicAssertions )
{
    auto dict = std::make_shared<PDFDictionary>( );

    auto domain = std::make_shared<PDFArray>( nullptr );
    domain->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    domain->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    domain->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    domain->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    dict->set( "Domain", domain );

    auto range = std::make_shared<PDFArray>( nullptr );
    range->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    range->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    dict->set( "Range", range );

    auto fn = std::make_shared<Function>( dict );
}

TEST( SampledFunctionConstructor, BasicAssertions )
{
    auto dict = std::make_shared<PDFDictionary>( );
    auto data = vrock::utils::from_hex_string_shared( "00" );

    auto domain = std::make_shared<PDFArray>( nullptr );
    domain->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    domain->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    domain->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    domain->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    dict->set( "Domain", domain );

    auto range = std::make_shared<PDFArray>( nullptr );
    range->value.push_back( std::make_shared<PDFReal>( -1.0 ) );
    range->value.push_back( std::make_shared<PDFReal>( 1.0 ) );
    dict->set( "Range", range );

    auto size = std::make_shared<PDFArray>( nullptr );
    size->value.push_back( std::make_shared<PDFInteger>( 21 ) );
    size->value.push_back( std::make_shared<PDFInteger>( 31 ) );
    dict->set( "Size", size );

    auto bps = std::make_shared<PDFInteger>( 8 );
    dict->set( "BitsPerSample", bps );

    auto stream = std::make_shared<PDFStream>( dict, data );

    auto fn = std::make_shared<SampledFunction>( stream );
}