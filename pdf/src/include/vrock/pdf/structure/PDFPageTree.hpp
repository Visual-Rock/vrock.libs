#pragma once

#include <vrock/utils/List.hpp>

#include "RenderableObject.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace vrock::pdf
{
    class PageTreeNode;

    class PageBaseObject : public PDFBaseObject
    {
    public:
        PageBaseObject( std::shared_ptr<PDFDictionary> dict, PageTreeNode *parent, std::shared_ptr<PDFContext> ctx,
                        bool leaf = true )
            : PDFBaseObject( leaf ? PDFObjectType::Page : PDFObjectType::PageTreeNode ), parent( parent ),
              context( std::move( ctx ) ), dictionary( std::move( dict ) )
        {
        }

    protected:
        std::shared_ptr<PDFContext> context;
        std::shared_ptr<PDFDictionary> dictionary;
        PageTreeNode *parent;

        auto get_property( const std::string &name ) -> std::shared_ptr<PDFBaseObject>;

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto get_property( const std::string &name ) -> std::shared_ptr<T>
        {
            return get_property( name )->to<T>( );
        }
    };

    class Page : public PageBaseObject, public Renderable
    {
    public:
        Page( std::shared_ptr<PDFDictionary>, std::shared_ptr<PDFContext>, PageTreeNode * );

        std::shared_ptr<Rectangle> media_box;
        std::shared_ptr<Rectangle> crop_box;
        std::shared_ptr<Rectangle> bleed_box;
        std::shared_ptr<Rectangle> trim_box;
        std::shared_ptr<Rectangle> art_box;

        std::int32_t rotation = 0;
    };

    class PageTreeNode : public PageBaseObject
    {
    public:
        PageTreeNode( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> context, PageTreeNode *parent );

        auto get_page( std::size_t idx ) -> std::shared_ptr<Page>;

        std::vector<std::shared_ptr<PageBaseObject>> kids = { };
        std::int32_t count = 0;
    };

    class PDFPageTree
    {
    public:
        PDFPageTree( ) = default;
        PDFPageTree( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> ctx );

        auto get_page( std::size_t idx ) -> std::shared_ptr<Page>;
        auto get_pages( ) -> utils::List<std::shared_ptr<Page>>;
        auto get_page_count( ) -> std::int32_t;

    private:
        utils::List<std::shared_ptr<Page>> pages;
        std::shared_ptr<PageTreeNode> page_tree;
        std::shared_ptr<PDFContext> context;
        bool all_pages_parsed = false;

        std::int32_t pages_count = 0;
    };

    template <>
    auto to_object_type<PageTreeNode>( ) -> PDFObjectType;

    template <>
    auto to_object_type<Page>( ) -> PDFObjectType;
} // namespace vrock::pdf