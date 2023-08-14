#include <iostream>
#include <memory>

import vrock.utils.ByteArray;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDocument;
import vrock.pdf.PDFEncryption;
using namespace vrock::pdf;

int main( )
{
    auto doc = PDFDocument( "./üser_öwner.pdf" );
    if ( doc.decryption_handler->is_encrypted( ) )
    {
        if ( auto handler = doc.decryption_handler->to<PDFStandardSecurityHandler, SecurityHandlerType::Standard>( ) )
        {
            while ( !handler->is_authenticated( ) )
                handler->authenticate( "öwner" );
        }
    }

    return 0;
}