#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace vrock::pdf
{
    using data_t = std::string;
    using in_data_t = std::string_view;

    class PDFDictionary;

    class BaseFilter
    {
    public:
        virtual auto encode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t = 0;

        virtual auto decode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t = 0;
    };

    class PDFASCIIFilter : public BaseFilter
    {
    public:
        auto encode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t final;

        auto decode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t final;
    };

    class PDFFlateFilter : public BaseFilter
    {
    public:
        auto encode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t final;

        auto decode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t final;
    };

    class PDFDCTFilter : public BaseFilter
    {
    public:
        auto encode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t final;

        auto decode( in_data_t, std::shared_ptr<PDFDictionary> ) -> data_t final;
    };

    inline std::unordered_map<std::string, std::shared_ptr<BaseFilter>> encodings = {
        { "ASCIIHexDecode", std::make_shared<PDFASCIIFilter>( ) },
        { "FlateDecode", std::make_shared<PDFFlateFilter>( ) },
        // TODO: implement JPXDecode and DCTDecode correctly
        { "DCTDecode", std::make_shared<PDFDCTFilter>( ) },
        { "JPXDecode", std::make_shared<PDFDCTFilter>( ) },
    };
} // namespace vrock::pdf