module;

import vrock.pdf.PDFEncryption;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFObjectParser;

#include <fstream>
#include <memory>
#include <string>
#include <utility>

module vrock.pdf.PDFDocument;

namespace vrock::pdf
{
    PDFDocument::PDFDocument( )
        : file_path( "" ), decryption_handler( std::make_shared<PDFNullSecurityHandler>( ) ),
          encryption_handler( decryption_handler )
    {
    }

    PDFDocument::PDFDocument( std::string path ) : file_path( std::move( path ) )
    {
        std::ifstream file( file_path, std::ifstream::binary );
        std::stringstream buffer;
        buffer << file.rdbuf( );
        file.close( );

        context = std::make_shared<PDFContext>( std::make_shared<PDFObjectParser>( buffer.str( ) ) );
        buffer.clear( );
        context->parser->set_context( context );
        context->init( );
    }
} // namespace vrock::pdf