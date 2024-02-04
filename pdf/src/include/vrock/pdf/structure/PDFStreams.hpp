#pragma once

#include <vrock/utils/List.hpp>

#include "PDFObjects.hpp"

#include "PDFContext.hpp"

namespace vrock::pdf
{
    class PDFObjectParser;

    enum class PDFStreamType
    {
        Raw,
        XRef,
        Object
    };

    template <typename T>
    auto to_stream_type( ) -> PDFStreamType
    {
        static_assert( false, "unknown conversion from type" );
    }

    class PDFStream : public PDFBaseObject
    {
    public:
        PDFStream( std::shared_ptr<PDFDictionary> dictionary, in_data_t _data, PDFStreamType t = PDFStreamType::Raw );

        template <typename T>
            requires std::is_base_of_v<PDFStream, T>
        auto to_stream( ) -> std::shared_ptr<T>
        {
            if ( to_stream_type<T>( ) == PDFStreamType::Raw )
                return as<T>( );
            if ( to_stream_type<T>( ) == stream_type )
                return as<T>( );
            return nullptr;
        }

        PDFStreamType stream_type;
        std::shared_ptr<PDFDictionary> dict;
        data_t data;

        utils::List<std::string> filters = { };
    };

    class PDFObjectStream : public PDFStream
    {
    public:
        explicit PDFObjectStream( std::shared_ptr<PDFDictionary> d, in_data_t data, std::shared_ptr<PDFContext> ctx );

        auto get_object( std::size_t idx ) -> std::shared_ptr<PDFBaseObject>;

    private:
        std::unordered_map<std::shared_ptr<PDFRef>, std::shared_ptr<PDFBaseObject>> objects = { };
        std::vector<std::pair<std::uint32_t, std::size_t>> offsets = { };
        std::shared_ptr<PDFObjectParser> parser;
        std::shared_ptr<PDFContext> context;
        std::size_t first = 0;
    };

    class PDFXRefStream : public PDFStream
    {
    public:
        explicit PDFXRefStream( std::shared_ptr<PDFDictionary> d, in_data_t data );

        auto get_entries( ) -> std::unordered_map<std::shared_ptr<XRefEntry>, std::shared_ptr<XRefEntry>,
                                                  XRefEntryPtrHash, XRefEntryPtrEqual>;
    };

    template <>
    auto to_object_type<PDFStream>( ) -> PDFObjectType;
    template <>
    auto to_stream_type<PDFStream>( ) -> PDFStreamType;
    template <>
    auto to_stream_type<PDFXRefStream>( ) -> PDFStreamType;
    template <>
    auto to_stream_type<PDFObjectStream>( ) -> PDFStreamType;
} // namespace vrock::pdf