module;

import vrock.pdf.PDFDocument;
import vrock.pdf.PDFPageTree;
import vrock.ui.FileDialog;
import vrock.ui.Image;

#include <memory>
#include <vector>

export module Globals;

export namespace globals
{
    inline std::vector<std::shared_ptr<vrock::pdf::PDFDocument>> documents = { };
    inline std::size_t selected_document = 0;
    inline std::vector<vrock::ui::Filter> pdf_file_filter = { { "PDF File", "*.pdf" } };
    inline std::vector<vrock::ui::Filter> image_file_filter = { { "PNG Image", "*.png" } };

    inline std::vector<std::shared_ptr<vrock::ui::Image>> rendered_pages;
    inline float scale = 1.0;
} // namespace globals
