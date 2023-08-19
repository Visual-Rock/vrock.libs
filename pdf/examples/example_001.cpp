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
    auto doc = PDFDocument( "/home/vrock/Downloads/0330-22-1.pdf" );

    if ( doc.decryption_handler->is_encrypted( ) )
        if ( auto handler = doc.decryption_handler->to<PDFStandardSecurityHandler>( ) )
            while ( !handler->is_authenticated( ) )
                handler->authenticate( "öwner" );

    return 0;
}