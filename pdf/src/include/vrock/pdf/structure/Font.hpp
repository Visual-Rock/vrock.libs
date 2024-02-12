#pragma once

#include "PDFDataStructures.hpp"
#include "PDFObjects.hpp"
#include "PDFStreams.hpp"
#include "Rectangle.hpp"

#include "vrock/pdf/parser/CMapParser.hpp"

namespace vrock::pdf
{
    enum class FontType
    {
        None,
        Type0,
        Type1,
        TrueType
    };

    enum class FontEncoding
    {
        None,
        StandardEncoding,
        MacRomanEncoding,
        WinAnsiEncoding,
        PDFDocEncoding,
        MacExpertEncoding
    };

    class FontDescriptor
    {
    public:
        FontDescriptor( std::shared_ptr<PDFDictionary> dict );
        ~FontDescriptor( ) = default;

        std::string font_name;
        std::string font_family;
        std::string font_stretch;
        std::int32_t font_weight = 0;
        std::int32_t flags = 0;
        std::shared_ptr<Rectangle> font_bbox = nullptr;
        double italic_angle = 0;
        std::int32_t ascent = 0;
        std::int32_t descent = 0;
        std::int32_t leading = 0;
        std::int32_t cap_height = 0;
        std::int32_t x_height = 0;
        std::int32_t stem_v = 0;
        std::int32_t stem_h = 0;
        std::int32_t avg_width = 0;
        std::int32_t max_width = 0;
        std::int32_t missing_width = 0;

        std::shared_ptr<PDFStream> font_file = nullptr;
        std::shared_ptr<PDFStream> font_file2 = nullptr;
        std::shared_ptr<PDFStream> font_file3 = nullptr;

        std::shared_ptr<PDFString> char_set = nullptr;
    };

    class Font : public std::enable_shared_from_this<Font>
    {
    public:
        Font( std::shared_ptr<PDFDictionary> dict );
        ~Font( ) = default;

        auto to_font_encoding( std::string_view str ) -> std::string;
        auto get_length( std::string_view str ) -> Unit;

        /// Subtype
        FontType font_type;

        std::string base_font;
        std::string font_name;

        std::int32_t first_char = 0;
        std::int32_t last_char = 0;

        std::vector<int32_t> widths = { };

        // TODO: Encoding
        FontEncoding encoding = FontEncoding::None;

        std::shared_ptr<FontDescriptor> descriptor = nullptr;
        std::shared_ptr<CMap> to_unicode;
    };

    auto convert_to_font_encoding( std::shared_ptr<Font>, std::string_view str ) -> std::string;
} // namespace vrock::pdf