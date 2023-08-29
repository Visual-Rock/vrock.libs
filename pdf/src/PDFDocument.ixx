module;

import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFPageTree;

export import vrock.pdf.PDFEncryption;

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

export module vrock.pdf.PDFDocument;

namespace vrock::pdf
{
    export enum class PDFSaveMode
    {
        /**
         * @brief Appends to the end of the PDF file (incremental update)
         * */
        Append // needs implementation
    };

    export class PDFDocument //:  std::enable_shared_from_this<PDFDocument>
    {
    public:
        PDFDocument( );
        PDFDocument( std::string path );

        // TODO: implement
        auto save( PDFSaveMode mode = PDFSaveMode::Append ) -> void;
        auto save( const std::string &path, PDFSaveMode mode = PDFSaveMode::Append ) -> void;

        auto get_path( ) -> std::string
        {
            return file_path;
        }

        auto get_file_name( ) -> std::string
        {
            std::filesystem::path p( file_path );
            return p.filename( );
        }

        auto get_pages( ) -> std::vector<std::shared_ptr<Page>>
        {
            return page_tree.get_pages( );
        }

        // TODO: abstract away?
        std::shared_ptr<PDFBaseSecurityHandler> decryption_handler;
        std::shared_ptr<PDFBaseSecurityHandler> encryption_handler;

    private:
        std::string file_path;
        PDFPageTree page_tree;
        // std::vector<std::shared_ptr<Page>> pages;

        std::shared_ptr<PDFContext> context;
    };
} // namespace vrock::pdf