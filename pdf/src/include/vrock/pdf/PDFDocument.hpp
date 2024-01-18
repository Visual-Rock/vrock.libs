#pragma once

#include "structure/PDFContext.hpp"
#include "structure/PDFEncryption.hpp"
#include "structure/PDFPageTree.hpp"

#include <filesystem>

namespace vrock::pdf
{
    enum class PDFSaveMode
    {
        /**
         * \brief Overwrites the file if it exists
         */
        Overwrite,
        /**
         * @brief Appends to the end of the PDF file (incremental update)
         * */
        Append // needs implementation
    };

    class PDFDocument //:  std::enable_shared_from_this<PDFDocument>
    {
    public:
        PDFDocument( );
        PDFDocument( std::filesystem::path path );

        // TODO: implement
        auto save( PDFSaveMode mode = PDFSaveMode::Overwrite ) -> void;
        auto save( std::filesystem::path path, PDFSaveMode mode = PDFSaveMode::Overwrite ) -> void;

        auto get_path( ) -> std::filesystem::path
        {
            return file_path;
        }

        auto get_file_name( ) const -> std::string
        {
            return file_path.filename( ).generic_string( );
        }

        auto get_page( std::size_t idx ) -> std::shared_ptr<Page>
        {
            return page_tree.get_page( idx );
        }

        auto get_pages( ) -> std::vector<std::shared_ptr<Page>>
        {
            return page_tree.get_pages( );
        }

        auto get_page_count( ) -> std::int32_t
        {
            return page_tree.get_page_count( );
        }

        // TODO: abstract away?
        std::shared_ptr<PDFBaseSecurityHandler> decryption_handler;
        std::shared_ptr<PDFBaseSecurityHandler> encryption_handler;

    private:
        std::filesystem::path file_path;
        PDFPageTree page_tree;
        // std::vector<std::shared_ptr<Page>> pages;

        std::shared_ptr<PDFContext> context;
    };
} // namespace vrock::pdf