#include "vrock/pdf/PDFDocument.hpp"

#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "vrock/pdf/parser/PDFObjectParser.hpp"

namespace vrock::pdf
{
    PDFDocument::PDFDocument( )
        : file_path( "" ), decryption_handler( std::make_shared<PDFNullSecurityHandler>( ) ),
          encryption_handler( decryption_handler )
    {
    }

    auto get_pages_( std::shared_ptr<PDFBaseObject> tree_node, std::vector<std::shared_ptr<Page>> &pages )
        -> std::vector<std::shared_ptr<Page>>
    {
        // for ( auto &kid : tree_node->kids )
        //     if ( auto page = kid->to<Page>( ) )
        //         pages.emplace_back( page );
        // else if ( auto node = kid->to<PageTreeNode>( ) )
        //     get_pages_( node, pages );
        // return pages;
        return { };
    }

    PDFDocument::PDFDocument( std::filesystem::path path ) : file_path( std::move( path ) )
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
            if ( auto root = context->trailer->get<PDFDictionary>( "Root" ) )
            {
                // load pages from Root dictionary
                if ( auto pages_root = root->get<PDFDictionary>( "Pages" ) )
                    page_tree = PDFPageTree( pages_root, context );
                else
                    throw std::runtime_error( "Pages is not a Dictionary" );
            }
            else
                throw std::runtime_error( "missing required Root entry in Trailer Dictionary" );
        };

        if ( auto encrypt = context->trailer->get<PDFDictionary>( "Encrypt" ) )
        {
            auto std_sec = std::make_shared<PDFStandardSecurityHandler>( encrypt, context, fn );
            decryption_handler = std_sec;
            context->parser->set_decryption_handler( decryption_handler );
            std_sec->has_user_password( );
        }
        else
        {
            decryption_handler = std::make_shared<PDFNullSecurityHandler>( );
            context->parser->set_decryption_handler( decryption_handler );
            fn( );
        }

        encryption_handler = decryption_handler;
    }

    auto PDFDocument::save( PDFSaveMode mode ) -> void
    {
    }

    auto PDFDocument::save( std::filesystem::path path, PDFSaveMode mode ) -> void
    {
    }
} // namespace vrock::pdf