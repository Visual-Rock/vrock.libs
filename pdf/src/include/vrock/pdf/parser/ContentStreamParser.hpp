#pragma once

#include "vrock/pdf/parser/PDFObjectParser.hpp"
#include "vrock/pdf/structure/Colorspaces.hpp"
#include "vrock/pdf/structure/Font.hpp"
#include "vrock/pdf/structure/PDFObjects.hpp"
#include "vrock/pdf/structure/Rectangle.hpp"
#include "vrock/pdf/structure/RenderableObject.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

namespace vrock::pdf
{
    typedef std::array<std::array<double, 3>, 3> mat3;

    enum class TextRenderingModes
    {
        FillText = 0,
        StrokeText = 1,
        FillThenStroke = 2,
        Invisible = 3,
        FillAndAddPath = 4,
        StrokeAndAddPath = 5,
        FillThenStrokeAndAddPath = 6,
        AddPath = 7
    };

    enum class LineCap
    {
        ButtCaps = 0,
        RoundCap = 1,
        ProjectingSquareCap = 2
    };

    enum class LineJoin
    {
        MiterdJoin = 0,
        RoundJoin = 1,
        BevelJoin = 2
    };

    enum class ContentStreamOperator : std::uint16_t
    {
        /// @brief Close, fill, and stroke path using non-zero winding number rule
        b,
        /// @brief Fill and stroke path using non-zero winding number rule
        B,
        /// @brief Close, fill, and stroke path using even-odd rule (b*)
        bs,
        /// @brief Fill and stroke path using even-odd rule (B*)
        Bs,
        /// @brief Begin marked-content sequence with property list
        BDC,
        /// @brief Begin inline image object
        BI,
        /// @brief Begin marked-content sequence
        BMC,
        /// @brief Begin text object
        BT,
        /// @brief Begin compatibility section
        BX,
        /// @brief Append curved segment to path (three control points)
        c,
        /// @brief Concatenate matrix to current transformation matrix
        cm,
        /// @brief Set colour space for stroking operations
        CS,
        /// @brief Set colour space for nonstroking operations
        cs,
        /// @brief Set line dash pattern
        d,
        /// @brief Set glyph width in Type 3 font
        d0,
        /// @brief Set glyph width and bounding box in Type 3 font
        d1,
        /// @brief Invoke named XObject
        Do,
        /// @brief Define marked-content point with property list
        DP,
        /// @brief End inline image object
        EI,
        /// @brief End marked-content sequence
        EMC,
        /// @brief End text object
        ET,
        /// @brief End compatibility section
        EX,
        /// @brief Fill path using non-zero winding number rule
        f,
        /// @brief Fill path using non-zero winding number rule (deprecated in PDF 2.0)
        F,
        /// @brief Fill path using even-odd rule
        fs,
        /// @brief Set gray level for stroking operations
        G,
        /// @brief Set gray level for nonstroking operations
        g,
        /// @brief (PDF 1.2) Set parameters from graphics state parameter dictionary
        gs,
        /// @brief Close subpath
        h,
        /// @brief Set flatness tolerance
        i,
        /// @brief Begin inline image data
        ID,
        /// @brief Set line join style
        j,
        /// @brief Set line cap style
        J,
        /// @brief Set CMYK colour for stroking operations
        k,
        /// @brief Append straight line segment to path
        l,
        /// @brief Begin new subpath
        m,
        /// @brief Set miter limit
        M,
        /// @brief (PDF 1.2) Define marked-content point
        MP,
        /// @brief End path without filling or stroking
        n,
        /// @brief Save graphics state
        q,
        /// @brief Restore graphics state
        Q,
        /// @brief Append rectangle to path
        re,
        /// @brief Set RGB colour for stroking operations
        RG,
        /// @brief Set RGB colour for nonstroking operations
        rg,
        /// @brief Set colour rendering intent
        ri,
        /// @brief Close and stroke path
        s,
        /// @brief Stroke path
        S,
        /// @brief (PDF 1.1) Set colour for stroking operations
        SC,
        /// @brief (PDF 1.1) Set colour for nonstroking operations
        sc,
        /// @brief (PDF 1.2) Set colour for stroking operations (ICCBased and special colour spaces)
        SCN,
        /// @brief (PDF 1.2) Set colour for nonstroking operations (ICCBased and special colour spaces)
        scn,
        /// @brief (PDF 1.3) Paint area defined by shading pattern
        sh,
        /// @brief Move to start of next text line (T*)
        Tst,
        /// @brief Set character spacing
        Tc,
        /// @brief Move text position
        Td,
        /// @brief Move text position and set leading
        TD,
        /// @brief Set text font and size
        Tf,
        /// @brief Show text
        Tj,
        /// @brief Show text, allowing individual glyph positioning
        TJ,
        /// @brief Set text leading
        TL,
        /// @brief Set text matrix and text line matrix
        Tm,
        /// @brief Set text rendering mode
        Tr,
        /// @brief Set text rise
        Ts,
        /// @brief Set word spacing
        Tw,
        /// @brief Set horizontal text scaling
        Tz,
        /// @brief Append curved segment to path (initial point replicated)
        v,
        /// @brief Set line width
        w,
        /// @brief Set clipping path using non-zero winding number rule
        W,
        /// @brief Set clipping path using even-odd rule
        Ws,
        /// @brief Append curved segment to path (final point replicated)
        y,
        /// @brief Move to next line and show text (')
        NextLine,
        /// @brief Set word and character spacing, move to next line, and show text (")
        SpacingNextLineShowText
    };

    inline std::unordered_map<std::string, ContentStreamOperator> string_to_operator = {
        { "b", ContentStreamOperator::b },        { "B", ContentStreamOperator::B },
        { "b*", ContentStreamOperator::bs },      { "B*", ContentStreamOperator::Bs },
        { "BDC", ContentStreamOperator::BDC },    { "BI", ContentStreamOperator::BI },
        { "BMC", ContentStreamOperator::BMC },    { "BT", ContentStreamOperator::BT },
        { "BX", ContentStreamOperator::BX },      { "c", ContentStreamOperator::c },
        { "cm", ContentStreamOperator::cm },      { "CS", ContentStreamOperator::CS },
        { "cs", ContentStreamOperator::cs },      { "d", ContentStreamOperator::d },
        { "d0", ContentStreamOperator::d0 },      { "d1", ContentStreamOperator::d1 },
        { "Do", ContentStreamOperator::Do },      { "DP", ContentStreamOperator::DP },
        { "EI", ContentStreamOperator::EI },      { "EMC", ContentStreamOperator::EMC },
        { "ET", ContentStreamOperator::ET },      { "EX", ContentStreamOperator::EX },
        { "f", ContentStreamOperator::f },        { "F", ContentStreamOperator::F },
        { "f*", ContentStreamOperator::fs },      { "G", ContentStreamOperator::G },
        { "g", ContentStreamOperator::g },        { "gs", ContentStreamOperator::gs },
        { "h", ContentStreamOperator::h },        { "i", ContentStreamOperator::i },
        { "ID", ContentStreamOperator::ID },      { "j", ContentStreamOperator::j },
        { "J", ContentStreamOperator::J },        { "k", ContentStreamOperator::k },
        { "l", ContentStreamOperator::l },        { "m", ContentStreamOperator::m },
        { "M", ContentStreamOperator::M },        { "MP", ContentStreamOperator::MP },
        { "n", ContentStreamOperator::n },        { "q", ContentStreamOperator::q },
        { "Q", ContentStreamOperator::Q },        { "re", ContentStreamOperator::re },
        { "RG", ContentStreamOperator::RG },      { "rg", ContentStreamOperator::rg },
        { "ri", ContentStreamOperator::ri },      { "s", ContentStreamOperator::s },
        { "S", ContentStreamOperator::S },        { "SC", ContentStreamOperator::SC },
        { "sc", ContentStreamOperator::sc },      { "SCN", ContentStreamOperator::SCN },
        { "scn", ContentStreamOperator::scn },    { "sh", ContentStreamOperator::sh },
        { "T*", ContentStreamOperator::Tst },     { "Tc", ContentStreamOperator::Tc },
        { "Td", ContentStreamOperator::Td },      { "TD", ContentStreamOperator::TD },
        { "Tf", ContentStreamOperator::Tf },      { "Tj", ContentStreamOperator::Tj },
        { "TJ", ContentStreamOperator::TJ },      { "TL", ContentStreamOperator::TL },
        { "Tm", ContentStreamOperator::Tm },      { "Tr", ContentStreamOperator::Tr },
        { "Ts", ContentStreamOperator::Ts },      { "Tw", ContentStreamOperator::Tw },
        { "Tz", ContentStreamOperator::Tz },      { "v", ContentStreamOperator::v },
        { "w", ContentStreamOperator::w },        { "W", ContentStreamOperator::W },
        { "W*", ContentStreamOperator::Ws },      { "y", ContentStreamOperator::y },
        { "'", ContentStreamOperator::NextLine }, { "\"", ContentStreamOperator::SpacingNextLineShowText }
    };

    class PDFOperator : public PDFBaseObject
    {
    public:
        PDFOperator( std::string op );
        ~PDFOperator( ) override = default;

        std::vector<std::shared_ptr<PDFBaseObject>> paramteres = { };
        std::string _operator;
        ContentStreamOperator o;
    };

    template <>
    auto to_object_type<PDFOperator>( ) -> PDFObjectType;

    // struct GraphicStateParameter
    // {
    //     double line_width = 0.0;
    //     int32_t line_cap_style = 0;
    //     int32_t line_join_style = 0;
    //     double miter_limiter = 0.0;

    //     std::vector<std::shared_ptr<PDFBaseObject>> line_dash_pattern = { };

    //     std::string rendering_intent = "";

    //     bool overprint_stroking = false;
    //     bool overprint = false;
    //     int32_t overprint_mode = 0;

    //     std::vector<std::shared_ptr<PDFBaseObject>> font;
    //     // BG
    //     // BG2
    //     // UCR
    //     // UCR2
    //     // TR
    //     // TR2
    //     // HT

    //     double flatness_tolerance = 0.0;
    //     double smoothness_tolerance = 0.0;
    //     bool automatic_stroke_adjustment = false;

    //     std::vector<std::shared_ptr<PDFBaseObject>> blend_mode = { };
    //     std::shared_ptr<PDFBaseObject> soft_mask;

    //     double current_stroking_alpha = 1.0;
    //     double current_alpha = 1.0;

    //     bool alpha_source_flag = false;
    //     bool text_knockout = false;

    //     std::string use_black_point_compensation = "Default";

    //     std::vector<std::shared_ptr<PDFBaseObject>> halftone_origin;
    // };

    struct TextState
    {
        double character_spacing = 0.0;
        double word_spacing = 0.0;
        double horizontal_scaling = 100.0f;
        int32_t leading = 100;
        std::shared_ptr<Font> font = nullptr;
        int32_t font_size = 0;
        TextRenderingModes text_render_mode = TextRenderingModes::FillText;
        double text_rise = 0.0;
        bool text_knockout = true;
    };

    class GraphicState
    {
    public:
        GraphicState( ) = default;
        ~GraphicState( ) = default;

        auto apply_dict( std::shared_ptr<PDFDictionary> ) -> void
        {
            // TODO: Apply
        }

        mat3 current_transformation_matrix{ { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } };
        std::shared_ptr<Rectangle> clipping_path = nullptr;
        //        std::shared_ptr<ColorSpace> color_space = std::make_shared<GrayColorSpace>( );
        // color
        TextState text_state;
        double line_width = 1.0;
        LineCap line_cap = LineCap::ButtCaps;
        LineJoin line_join = LineJoin::MiterdJoin;
        double miter_limit = 10.0;
        std::pair<std::vector<double>, double> dash_pattern = { { }, 0.0 };
        std::string rendering_intent = "RelativeColorimetric";
        bool stroke_adjustment = false;
        std::vector<std::string> blend_mode = { "Normal" };
        /// @brief nullptr corresponds to None
        std::shared_ptr<PDFDictionary> soft_mask = nullptr;
        double alpha_constant = 1.0;
        bool alpha_source = false;
        std::string black_point_compensation = "Default";
        bool overprint = false;
        double overprint_mode = 0.0;
        // TODO: to std::variant with functiontype + initialise value
        std::string black_generation;
        std::string undercolor_removal;
        std::shared_ptr<PDFBaseObject> halftone = nullptr;

        double flatness = 1.0;
        double smoothness = 0.1;
    };

    class ContentStreamParser : public PDFObjectParser
    {
    public:
        ContentStreamParser( std::string data, std::shared_ptr<ResourceDictionary> res_dict,
                             std::shared_ptr<PDFContext> ctx );

        auto parse_object( std::shared_ptr<PDFRef> ref = nullptr, bool decrypt = false )
            -> std::shared_ptr<PDFBaseObject>;

        auto parse_operator( ) -> std::shared_ptr<PDFOperator>;

        utils::List<std::shared_ptr<Image>> images = { };
        utils::List<std::shared_ptr<Text>> text = { };

    private:
        auto to_operator_array( ) -> std::vector<std::shared_ptr<PDFOperator>>;

    public:
        std::shared_ptr<ResourceDictionary> res;
        std::stack<GraphicState> graphic_state_stack = { };
        std::shared_ptr<Text> current_text_section = nullptr;
    };

    // operator functions
    // void operator_b( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_BT( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_cm( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_Do( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_ET( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_gs( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_q( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_Q( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_Tc( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_Tf( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_Tj( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_TJ( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_TL( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    void operator_Tw( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_Tz( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_v( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_w( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_W( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_Ws( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_y( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_NL( ContentStreamParser *, std::shared_ptr<PDFOperator> op );
    void operator_SNLST( ContentStreamParser *, std::shared_ptr<PDFOperator> op );

    inline std::unordered_map<ContentStreamOperator,
                              std::function<void( ContentStreamParser *, std::shared_ptr<PDFOperator> )>>
        operator_fn = {
            { ContentStreamOperator::BT, operator_BT }, { ContentStreamOperator::cm, operator_cm }, 
            { ContentStreamOperator::Do, operator_Do }, { ContentStreamOperator::ET, operator_ET },
            { ContentStreamOperator::gs, operator_gs }, { ContentStreamOperator::q, operator_q },
            { ContentStreamOperator::Q, operator_Q },   { ContentStreamOperator::Tf, operator_Tf },
            { ContentStreamOperator::Tc, operator_Tc }, { ContentStreamOperator::Tj, operator_Tj },
            { ContentStreamOperator::TJ, operator_TJ }, { ContentStreamOperator::TL, operator_TL },
            { ContentStreamOperator::Tw, operator_Tw }, { ContentStreamOperator::Tz, operator_Tz },
            { ContentStreamOperator::w, operator_w },
        };

} // namespace vrock::pdf