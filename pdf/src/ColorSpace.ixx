module;

import vrock.utils.ByteArray;
import vrock.pdf.PDFBaseObjects;

#include <cstdint>
#include <memory>

export module vrock.pdf.ColorSpace;

namespace vrock::pdf
{
    export enum class ColorSpaceType
    {
        Indexed
    };

    export class ColorSpace
    {
    public:
        ColorSpace( ColorSpaceType t ) : type( t )
        {
        }

        virtual auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFBaseObject> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        ColorSpaceType type;
    };

    export class IndexedColorSpace : public ColorSpace
    {
    public:
        explicit IndexedColorSpace( std::shared_ptr<PDFArray> );

        auto convert_to_rgb( std::shared_ptr<utils::ByteArray<>> ptr, std::shared_ptr<PDFBaseObject> sharedPtr )
            -> std::shared_ptr<utils::ByteArray<>>;

        std::int32_t highest_value = 0;
        std::shared_ptr<utils::ByteArray<>> map = nullptr;
    };

    export auto to_colorspace( std::shared_ptr<PDFBaseObject> ) -> std::shared_ptr<ColorSpace>;
} // namespace vrock::pdf
