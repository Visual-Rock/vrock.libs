module;

import vrock.pdf.PDFBaseObjects;
export import vrock.pdf.PDFEncryption;

#include <memory>
#include <string>

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

    export class PDFDocument
    {
    public:
        PDFDocument( );
        PDFDocument( std::string path );

        auto save( PDFSaveMode mode = PDFSaveMode::Append ) -> void;
        auto save( const std::string &path, PDFSaveMode mode = PDFSaveMode::Append ) -> void;

        const std::shared_ptr<PDFBaseSecurityHandler> decryption_handler;
        std::shared_ptr<PDFBaseSecurityHandler> encryption_handler;

    private:
        std::string file_path;

        std::shared_ptr<PDFContext> context;
    };

} // namespace vrock::pdf