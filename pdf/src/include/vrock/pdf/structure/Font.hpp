#pragma once

#include "PDFObjects.hpp"

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

    class Font
    {
    public:
        Font( std::shared_ptr<PDFDictionary> dict );
        ~Font( ) = default;

        /// Subtype
        FontType font_type;

        std::string base_font;
        std::string font_name;

        std::int32_t first_char = 0;
        std::int32_t last_char = 0;

        std::vector<int32_t> widths = { };

        FontEncoding encoding = FontEncoding::None;

        // std::shared_ptr<FontDescriptor> descriptor = nullptr;

        // TODO: Encoding

        // std::shared_ptr<structure::ToUnicode> to_unicode;
    };

    auto convert_to_font_encoding( std::shared_ptr<Font>, std::string_view str ) -> std::string;
} // namespace vrock::pdf