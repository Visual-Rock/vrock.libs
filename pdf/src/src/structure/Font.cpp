#include "vrock/pdf/structure/Font.hpp"

namespace vrock::pdf
{
    static std::unordered_map<std::string, FontType> font_types = { { "Type1", FontType::Type1 },
                                                                    { "TrueType", FontType::TrueType } };

    Font::Font( std::shared_ptr<PDFDictionary> dict )
    {
        if ( auto type = dict->get<PDFName>( "Subtype" ) )
            if ( font_types.contains( type->name ) )
                font_type = font_types[ type->name ];

        if ( auto first = dict->get<PDFInteger>( "FirstChar" ) )
            first_char = first->value;
        if ( auto last = dict->get<PDFInteger>( "FirstChar" ) )
            last_char = last->value;
        if ( auto base = dict->get<PDFName>( "BaseFont" ) )
            base_font = base->name;
        if ( auto name = dict->get<PDFName>( "Name" ) )
            font_name = name->name;
        if ( auto w = dict->get<PDFArray>( "Widths" ) )
        {
            widths.reserve( w->value.size( ) );
            for ( const auto &v : w->value )
                if ( auto val = v->to<PDFInteger>( ) )
                    widths.emplace_back( val->value );
        }
    }
} // namespace vrock::pdf