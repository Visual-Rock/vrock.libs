#pragma once

#include <vrock/utils/List.hpp>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Font.hpp"
#include "PDFDataStructures.hpp"
#include "PDFImage.hpp"
#include "PDFObjects.hpp"
#include "PDFStreams.hpp"
#include "Rectangle.hpp"

namespace vrock::pdf
{
    enum class RenderableObjectType
    {
        None,
        Image,
        Text
    };

    template <typename T>
    auto to_renderable_object_type( ) -> RenderableObjectType
    {
        return RenderableObjectType::None;
    }

    class RenderableObject : public std::enable_shared_from_this<RenderableObject>
    {
    public:
        RenderableObject( RenderableObjectType t, std::int64_t z );

        OPTNONE_START bool is( RenderableObjectType t )
        {
            if ( this == nullptr )
                return false;
            return t == type;
        }
        OPTNONE_END

        template <class T>
            requires std::is_base_of_v<RenderableObject, T>
        OPTNONE_START std::shared_ptr<T> as( )
        {
            if ( this == nullptr )
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
        }
        OPTNONE_END

        template <typename T>
            requires std::is_base_of_v<PDFBaseObject, T>
        auto to( ) -> std::shared_ptr<T>
        {
            if ( is( to_renderable_object_type<T>( ) ) )
                return as<T>( );
            return nullptr;
        }

        std::int64_t z_index;
        RenderableObjectType type;
    };

    class Image : public RenderableObject
    {
    public:
        Image( );

        explicit Image( std::int64_t z );

        Point position, scale;
        double shear = 0, rotation = 0;
        std::shared_ptr<PDFImage> image;
    };

    class Text : public RenderableObject
    {
    public:
        Text( );

        explicit Text( std::int64_t z );

        std::string text;
        /// @brief List of offsets. the pairs are structured in the following way:
        ///         substring length, offset
        std::vector<std::pair<int32_t, int32_t>> offsets;

        /// @brief font size in pt
        std::int32_t font_size = 0;
        std::shared_ptr<Font> font;
    };

    class Form;

    class ResourceDictionary
    {
    public:
        ResourceDictionary( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> ctx );
        ~ResourceDictionary( ) = default;

        auto load_xobject( std::shared_ptr<PDFDictionary> dict ) -> void;

        std::unordered_map<std::string, std::shared_ptr<PDFDictionary>> ext_g_state = { };
        // XObjects
        std::unordered_map<std::string, std::shared_ptr<PDFImage>> images = { };
        std::unordered_map<std::string, std::shared_ptr<Font>> fonts = { };
        std::unordered_map<std::string, std::shared_ptr<Form>> forms = { };

    private:
        std::shared_ptr<PDFDictionary> dict;
        std::shared_ptr<PDFContext> context;
    };

    template <>
    auto to_renderable_object_type<Image>( ) -> RenderableObjectType;

    template <>
    auto to_renderable_object_type<Text>( ) -> RenderableObjectType;

    class Renderable
    {
    public:
        explicit Renderable( std::shared_ptr<PDFContext> ctx );

        auto get_images( ) -> utils::List<std::shared_ptr<Image>>;

        auto get_text( ) -> utils::List<std::shared_ptr<Text>>;

        std::shared_ptr<ResourceDictionary> resources = nullptr;

    protected:
        bool parsed = false;
        utils::List<std::shared_ptr<Image>> images = { };
        utils::List<std::shared_ptr<Text>> text = { };
        std::vector<std::shared_ptr<PDFStream>> content_streams = { };

        std::shared_ptr<PDFContext> context;

    protected:
        auto parse_content( ) -> void;
    };

    class Form : public Renderable
    {
    public:
        Form( std::shared_ptr<PDFStream> form, std::shared_ptr<PDFContext> ctx );

        std::shared_ptr<Rectangle> bbox;
    };
} // namespace vrock::pdf