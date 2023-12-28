#include <chrono>
#include <iostream>

#include "vrock/pdf/PDFDocument.hpp"
#include "vrock/utils.hpp"

using namespace vrock::pdf;

int main( )
{
    vrock::utils::ScopedTimer timer( []( auto t ) { std::cout << t << "ms" << std::endl; } );

    auto doc = PDFDocument( "./image.pdf" );

    if ( doc.decryption_handler->is_encrypted( ) )
        if ( auto handler = doc.decryption_handler->to<PDFStandardSecurityHandler>( ) )
            while ( !handler->is_authenticated( ) )
                handler->authenticate( "öwner" );

    std::size_t i = 0;
    for ( const auto &page : doc.get_pages( ) )
        for ( auto img : page->get_images( ) )
        {
        }

    return 0;
}