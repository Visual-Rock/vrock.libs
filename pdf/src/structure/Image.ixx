module;

import vrock.pdf.PDFDataStructures;
import vrock.pdf.PDFBaseObjects;

#include <memory>

export module vrock.pdf.Image;

namespace vrock::pdf
{
    export class Image
    {
    public:
        Image( ) = default;

        Point position, size;
        std::shared_ptr<PDFImage> image_data;
    };
} // namespace vrock::pdf