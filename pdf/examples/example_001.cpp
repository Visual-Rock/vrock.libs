#include <chrono>
#include <iostream>
#include <memory>

import vrock.utils.ByteArray;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDocument;
import vrock.pdf.PDFEncryption;
import vrock.utils.Timer;
using namespace vrock::pdf;

int main( )
{
    vrock::utils::ScopedTimer timer( []( auto t ) { std::cout << t << "ms" << std::endl; } );
    auto doc = PDFDocument( "./üser_öwner.pdf" );

    if ( doc.decryption_handler->is_encrypted( ) )
        if ( auto handler = doc.decryption_handler->to<PDFStandardSecurityHandler>( ) )
            while ( !handler->is_authenticated( ) )
                handler->authenticate( "öwner" );

    std::size_t i = 0;
    for ( const auto &page : doc.get_pages( ) )
        for ( auto [ key, img ] : page->resources->images )
            img->save( std::format( "./{}.png", i++ ) );

    return 0;
}