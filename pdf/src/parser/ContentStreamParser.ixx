module;

import vrock.pdf.PDFObjectParser;
import vrock.pdf.PDFBaseObjects;

#include <memory>
#include <vector>

export module vrock.pdf.ContentStreamParser;

namespace vrock::pdf
{
    export enum class TextRenderingModes
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

    export enum class LineCap
    {
        ButtCaps = 0,
        RoundCap = 1,
        ProjectingSquareCap = 2
    };

    export enum class LineJoin
    {
        MiterdJoin = 0,
        RoundJoin = 1,
        BevelJoin = 2
    };

    // struct GraphicStateParameter
    // {
    //     double line_width = 0.0;
    //     int32_t line_cap_style = 0;
    //     int32_t line_join_style = 0;
    //     double miter_limiter = 0.0;

    //     std::vector<std::shared_ptr<objects::PDFBaseObject>> line_dash_pattern = { };

    //     std::string rendering_intent = "";

    //     bool overprint_stroking = false;
    //     bool overprint = false;
    //     int32_t overprint_mode = 0;

    //     std::vector<std::shared_ptr<objects::PDFBaseObject>> font;
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

    //     std::vector<std::shared_ptr<objects::PDFBaseObject>> blend_mode = { };
    //     std::shared_ptr<objects::PDFBaseObject> soft_mask;

    //     double current_stroking_alpha = 1.0;
    //     double current_alpha = 1.0;

    //     bool alpha_source_flag = false;
    //     bool text_knockout = false;

    //     std::string use_black_point_compensation = "Default";

    //     std::vector<std::shared_ptr<objects::PDFBaseObject>> halftone_origin;
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

        double current_transformation_matrix[ 3 ][ 3 ] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
        std::shared_ptr<Rectangle> clipping_path = nullptr;
        std::vector<std::string> color_space = { "DeviceGray" };
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

    export class ContentStreamParser : public PDFObjectParser
    {
        ContentStreamParser( std::string data, std::shared_ptr<ResourceDictionary> res_dict,
                             std::shared_ptr<PDFContext> ctx );

        auto parse_object( std::shared_ptr<PDFRef> ref = nullptr, bool decrypt = false )
            -> std::shared_ptr<PDFBaseObject>;
    };
} // namespace vrock::pdf