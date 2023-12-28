#pragma once

#include <vrock/utils/ByteArray.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace vrock::pdf
{
    class PDFDictionary;

    class BaseFilter
    {
    public:
        virtual auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;

        virtual auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;
    };

    class PDFASCIIFilter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    class PDFFlateFilter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    class PDFDCTFilter : public BaseFilter
    {
    public:
        auto encode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto decode( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFDictionary> )
            -> std::shared_ptr<utils::ByteArray<>> final;
    };

    inline std::unordered_map<std::string, std::shared_ptr<BaseFilter>> encodings = {
        { "ASCIIHexDecode", std::make_shared<PDFASCIIFilter>( ) },
        { "FlateDecode", std::make_shared<PDFFlateFilter>( ) },
        // TODO: implement JPXDecode and DCTDecode correctly
        { "DCTDecode", std::make_shared<PDFDCTFilter>( ) },
        { "JPXDecode", std::make_shared<PDFDCTFilter>( ) },
    };
} // namespace vrock::pdf