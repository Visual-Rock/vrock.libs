module;

import vrock.pdf.PDFDataStructures;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDataStructures;
import vrock.utils.List;

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

export module vrock.pdf.RenderableObject;

namespace vrock::pdf
{
    export enum class RenderableObjectType
    {
        Image,
        Text
    };

    export template <typename T>
    auto to_renderable_object_type( ) -> RenderableObjectType
    {
        static_assert( "unknown conversion from type" );
    }

    export class RenderableObject : public std::enable_shared_from_this<RenderableObject>
    {
    public:
        RenderableObject( RenderableObjectType t, std::int64_t z ) : z_index( z ), type( t )
        {
        }

        bool is( RenderableObjectType t ) __attribute__( ( optnone ) )
        {
            if ( this == nullptr )
                return false;
            return t == type;
        }

        template <class T>
            requires std::is_base_of_v<RenderableObject, T>
        std::shared_ptr<T> as( ) __attribute__( ( optnone ) )
        {
            if ( this == nullptr )
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
        }

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

    protected:
    };

    export class Image : public RenderableObject
    {
    public:
        Image( ) : RenderableObject( RenderableObjectType::Image, 0 )
        {
        }

        explicit Image( std::int64_t z ) : RenderableObject( RenderableObjectType::Image, z )
        {
        }

        Point position, scale;
        double shear = 0, rotation = 0;
        std::shared_ptr<PDFImage> image;
    };

    export class Text : public RenderableObject
    {
    public:
        Text( ) : RenderableObject( RenderableObjectType::Text, 0 )
        {
        }

        explicit Text( std::int64_t z ) : RenderableObject( RenderableObjectType::Text, z )
        {
        }

        std::string text;
        /// @brief List of offsets. the pairs are structured in the following way:
        ///         substring length, offset
        std::vector<std::pair<int32_t, int32_t>> offsets;

        /// @brief font size in pt
        std::int32_t font_size = 0;
        std::shared_ptr<Font> font;
    };

    export class Form;

    export class ResourceDictionary
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
    auto to_renderable_object_type<Image>( ) -> RenderableObjectType
    {
        return RenderableObjectType::Image;
    }

    export class Renderable
    {
    public:
        explicit Renderable( std::shared_ptr<PDFContext> ctx ) : context( std::move( ctx ) )
        {
        }

        auto get_images( ) -> utils::List<std::shared_ptr<Image>>
        {
            parse_content( );
            return images;
        }

        auto get_text( ) -> utils::List<std::shared_ptr<Text>>
        {
            parse_content( );
            return text;
        }

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
        Form( std::shared_ptr<PDFStream> form, std::shared_ptr<PDFContext> ctx ) : Renderable( std::move( ctx ) )
        {
            if ( auto array = form->dict->get<PDFArray>( "BBox" ) )
                bbox = std::make_shared<Rectangle>( array );
            content_streams.push_back( form );
            if ( auto res = form->dict->get<PDFDictionary>( "Resources" ) )
                resources = std::make_shared<ResourceDictionary>( res, context );
        }

        std::shared_ptr<Rectangle> bbox;
    };

} // namespace vrock::pdf
