#include <iostream>
#include <memory>

import vrock.utils.ByteArray;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDocument;
import vrock.pdf.PDFEncryption;
using namespace vrock::pdf;

int main( )
{
    auto doc = PDFDocument( "./with_update_sections.pdf" );
    if ( doc.decryption_handler->is_encrypted( ) )
    {
        if ( doc.decryption_handler->is( PDFSecurityHandlerType::Standard ) )
        {
            auto handler = doc.decryption_handler->as<PDFStandardSecurityHandler>( );
            while ( !handler->is_authenticated( ) )
                handler->authenticate( "" );
        }
    }

    return 0;
}