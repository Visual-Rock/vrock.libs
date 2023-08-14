module;

import vrock.pdf.PDFEncryption;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFObjectParser;

#include <fstream>
#include <functional>
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

        std::function<void( )> fn = [ this ]( ) {
            if ( auto root = context->trailer->get<PDFDictionary, PDFObjectType::Dictionary>( "Root" ) )
            {
            }
            else
                throw std::runtime_error( "missing required Root entry in Trailer Dictionary" );
        };

        if ( auto encrypt = context->trailer->get<PDFDictionary, PDFObjectType::Dictionary>( "Encrypt" ) )
        {
            auto std_sec = std::make_shared<PDFStandardSecurityHandler>( encrypt, context, fn );
            decryption_handler = std_sec;
            std_sec->has_user_password( );
        }
        else
        {
            decryption_handler = std::make_shared<PDFNullSecurityHandler>( );
            context->parser->set_decryption_handler( decryption_handler );
            fn( );
        }
        context->parser->set_decryption_handler( decryption_handler );
        encryption_handler = decryption_handler;
    }
} // namespace vrock::pdf