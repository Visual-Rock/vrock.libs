#pragma once

#include "PDFStreams.hpp"

namespace vrock::pdf
{
    enum class FunctionType : std::int32_t
    {
        Sampled = 0,
        ExponentialInterpolation = 2,
        Stitching = 3,
        PostScriptCalculator = 4
    };

    class Function
    {
    public:
        explicit Function( std::shared_ptr<PDFDictionary> dict );

        FunctionType type;
        std::vector<std::shared_ptr<PDFNumber>> domain;
        std::vector<std::shared_ptr<PDFNumber>> range;

        std::shared_ptr<PDFDictionary> dict;
    };

    // ISO: 7.10.2 Type 0 (sampled) functions
    class SampledFunction : public Function
    {
    public:
        explicit SampledFunction( std::shared_ptr<PDFStream> stream );

        auto sample( std::vector<float> numbers ) -> std::vector<float>;

        std::vector<std::int32_t> size = { };
    };

    auto make_function( std::shared_ptr<PDFBaseObject> object ) -> std::shared_ptr<Function>;
} // namespace vrock::pdf