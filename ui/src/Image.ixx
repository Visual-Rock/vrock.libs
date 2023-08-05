module;

#include <cstdint>
#include <string>

#include "vulkan/vulkan.h"

export module vrock.ui.Image;

namespace vrock::ui
{
    export enum class ImageFormat
    {
        None = 0,
        RGBA,
        RGBA32F
    };

    /**
     * @class Image
     * @brief This class represents an image file, providing various controls such as resizing, modifying data, etc.
     */
    export class Image
    {
    public:
        /**
         * @brief Construct Image from file path.
         * @param path File location of the image.
         */
        explicit Image( const std::string &path );

        /**
         * @brief Construct Image from raw data.
         * @param width Width of the image.
         * @param height Height of the image.
         * @param format Image format.
         * @param data Pointer to the raw image data.
         */
        Image( std::uint32_t, std::uint32_t, ImageFormat, const void *data );

        /// @brief Destructor.
        ~Image( );

        /**
         * @brief Resize the image.
         * @param width New width of the image.
         * @param height New height of the image.
         */
        auto resize( std::uint32_t, std::uint32_t ) -> void;

        /**
         * @brief Sets new image data.
         * @param data Pointer to the new raw image data.
         */
        auto set_data( const void *data ) -> void;

        /**
         * @brief Retrieve Vulkan descriptor set for the image.
         * @return Descriptor set for the image.
         */
        [[nodiscard]] auto get_descriptor_set( ) const -> VkDescriptorSet;

        /**
         * @brief Get the width of the image.
         * @return Width of the image.
         */
        auto get_width( ) const -> std::uint32_t;

        /**
         * @brief Get the height of the image.
         * @return Height of the image.
         */
        auto get_height( ) const -> std::uint32_t;

    private:
        auto allocate( std::size_t ) -> void;
        auto release( ) -> void;

        std::string filepath;
        std::uint32_t width = 0, height = 0;

        ImageFormat format = ImageFormat::None;

        VkImage image = nullptr;
        VkImageView image_view = nullptr;
        VkDeviceMemory memory = nullptr;
        VkSampler sampler = nullptr;

        VkBuffer buffer = nullptr;
        VkDeviceMemory device_memory = nullptr;
        std::size_t aligned_size = 0;

        VkDescriptorSet descriptor = nullptr;
    };
} // namespace vrock::ui