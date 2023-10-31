#pragma once

#include "PDFObjects.hpp"

#include <vrock/utils/List.hpp>

namespace vrock::pdf
{
    class PDFObjectParser;

    class XRefEntry
    {
    public:
        XRefEntry( ) = default;
        XRefEntry( std::uint32_t offset, std::uint32_t object_number, std::uint32_t gen_num, std::uint8_t type );

        std::uint32_t offset = 0;
        std::uint32_t object_number = 0;
        std::uint32_t generation_number = 0;
        std::uint8_t type = 0;
    };

    struct XRefEntryPtrHash
    {
    public:
        size_t operator( )( const std::shared_ptr<XRefEntry> &r ) const
        {
            return std::hash<std::uint64_t>( )( ( (uint64_t)r->object_number ) << 32 | r->generation_number );
        }
    };

    struct XRefEntryPtrEqual
    {
        bool operator( )( const std::shared_ptr<XRefEntry> &l, const std::shared_ptr<XRefEntry> &r ) const
        {
            return l->object_number == r->object_number && l->generation_number == r->generation_number;
        }
    };

    class XRefTable
    {
    public:
        XRefTable( ) = default;

        auto get_entry( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<XRefEntry>;

        std::unordered_map<std::shared_ptr<XRefEntry>, std::shared_ptr<XRefEntry>, XRefEntryPtrHash, XRefEntryPtrEqual>
            entries = { };
        std::shared_ptr<PDFDictionary> trailer;
    };

    class PDFContext
    {
    public:
        PDFContext( std::shared_ptr<PDFObjectParser> parser );

        auto get_object( const std::shared_ptr<PDFRef> & ) -> std::shared_ptr<PDFBaseObject>;

        auto init( ) -> void;

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get_object( const std::shared_ptr<PDFRef> &ref ) -> std::shared_ptr<T>
        {
            return get_object( ref )->to<T>( );
        }

        std::shared_ptr<PDFObjectParser> parser;
        std::unordered_map<std::shared_ptr<PDFRef>, std::shared_ptr<PDFBaseObject>, PDFRefPtrHash, PDFRefPtrEqual>
            objects = { };

        std::shared_ptr<PDFDictionary> trailer;
        std::vector<std::shared_ptr<XRefTable>> xref_tables;
    };
} // namespace vrock::pdf