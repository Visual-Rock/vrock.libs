#pragma once

#include "PDFStreams.hpp"

namespace vrock::pdf
{
    enum class FunctionType : std::int32_t
    {
        Identity = -1,
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

    protected:
        std::int32_t m, n;
        std::vector<std::pair<double, double>> domain = { };
        std::vector<std::pair<double, double>> range = { };

        std::shared_ptr<PDFDictionary> dict;
    };

    // ISO: 7.10.2 Type 0 (sampled) functions
    class SampledFunction : public Function
    {
    public:
        explicit SampledFunction( std::shared_ptr<PDFStream> stream );

        auto sample( std::vector<float> numbers ) -> std::vector<float>;

    private:
        std::vector<std::int32_t> size = { };
        std::vector<std::int32_t> offsets = { };
        std::int32_t bits_per_sample = 8;

        std::vector<std::pair<double, double>> encode = { };
        std::vector<double> input_multipliers = { };
        std::vector<std::pair<double, double>> decode = { };
    };

    auto make_function( std::shared_ptr<PDFBaseObject> object ) -> std::shared_ptr<Function>;
} // namespace vrock::pdf