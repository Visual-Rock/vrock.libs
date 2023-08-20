module;

#include <cstdint>
#include <string>

#include "vulkan/vulkan.h"

#include "stb_image.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

module vrock.ui.Image;

import vrock.ui.Application;

using namespace vrock::ui::internal;

namespace vrock::ui
{
    auto bytes_per_pixel( const ImageFormat &format ) -> std::uint32_t
    {
        switch ( format )
        {
        case ImageFormat::RGBA:
            return 4;
        case ImageFormat::RGBA32F:
            return 16;
        default:
            return 0;
        }
    }

    auto to_vulkan_image_format( const ImageFormat &format ) -> VkFormat
    {
        switch ( format )
        {
        case ImageFormat::RGBA:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::RGBA32F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:
            return (VkFormat)0;
        }
    }

    auto vulkan_memory_type( VkMemoryPropertyFlags properties, std::uint32_t type_bits ) -> std::uint32_t
    {
        VkPhysicalDeviceMemoryProperties prop;
        vkGetPhysicalDeviceMemoryProperties( get_physical_device( ), &prop );
        for ( uint32_t i = 0; i < prop.memoryTypeCount; i++ )
            if ( ( prop.memoryTypes[ i ].propertyFlags & properties ) == properties && type_bits & ( 1 << i ) )
                return i;

        return 0xffffffff;
    }

    Image::Image( const std::string &path )
    {
        int w, h, c;
        std::uint8_t *data = nullptr;

        if ( stbi_is_hdr( path.c_str( ) ) )
        {
            data = (std::uint8_t *)stbi_loadf( path.c_str( ), &w, &h, &c, 4 );
            format = ImageFormat::RGBA32F;
        }
        else
        {
            data = stbi_load( path.c_str( ), &w, &h, &c, 4 );
            format = ImageFormat::RGBA;
        }

        width = w;
        height = h;

        allocate( width * height * bytes_per_pixel( format ) );
        set_data( data );
        stbi_image_free( data );
    }

    Image::Image( std::uint32_t w, std::uint32_t h, ImageFormat f, const void *data )
        : width( w ), height( h ), format( f )
    {
        allocate( width * height * bytes_per_pixel( format ) );
        if ( data )
            set_data( data );
    }

    Image::~Image( )
    {
        release( );
    }

    auto Image::resize( std::uint32_t w, std::uint32_t h ) -> void
    {
        if ( image && width == w && height == h )
            return;

        // TODO: max size?

        width = w;
        height = h;

        release( );
        allocate( width * height * bytes_per_pixel( format ) );
    }

    auto Image::set_data( const void *data ) -> void
    {
        VkDevice device = internal::get_device( );

        size_t upload_size = width * height * bytes_per_pixel( format );

        VkResult err;

        if ( !buffer )
        {
            // Create the Upload Buffer
            {
                VkBufferCreateInfo buffer_info = { };
                buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                buffer_info.size = upload_size;
                buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                err = vkCreateBuffer( device, &buffer_info, nullptr, &buffer );
                check_vk_result( err );
                VkMemoryRequirements req;
                vkGetBufferMemoryRequirements( device, buffer, &req );
                aligned_size = req.size;
                VkMemoryAllocateInfo alloc_info = { };
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.allocationSize = req.size;
                alloc_info.memoryTypeIndex =
                    vulkan_memory_type( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits );
                err = vkAllocateMemory( device, &alloc_info, nullptr, &device_memory );
                check_vk_result( err );
                err = vkBindBufferMemory( device, buffer, device_memory, 0 );
                check_vk_result( err );
            }
        }

        // Upload to Buffer
        {
            char *map = nullptr;
            err = vkMapMemory( device, device_memory, 0, aligned_size, 0, (void **)( &map ) );
            check_vk_result( err );
            memcpy( map, data, upload_size );
            VkMappedMemoryRange range[ 1 ] = { };
            range[ 0 ].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[ 0 ].memory = device_memory;
            range[ 0 ].size = aligned_size;
            err = vkFlushMappedMemoryRanges( device, 1, range );
            check_vk_result( err );
            vkUnmapMemory( device, device_memory );
        }

        // Copy to Image
        {
            VkCommandBuffer command_buffer = get_command_buffer( );

            VkImageMemoryBarrier copy_barrier = { };
            copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier.image = image;
            copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_barrier.subresourceRange.levelCount = 1;
            copy_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier( command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                  nullptr, 0, nullptr, 1, &copy_barrier );

            VkBufferImageCopy region = { };
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.layerCount = 1;
            region.imageExtent.width = width;
            region.imageExtent.height = height;
            region.imageExtent.depth = 1;
            vkCmdCopyBufferToImage( command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );

            VkImageMemoryBarrier use_barrier = { };
            use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier.image = image;
            use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            use_barrier.subresourceRange.levelCount = 1;
            use_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier( command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                  0, 0, nullptr, 0, nullptr, 1, &use_barrier );

            flush_command_buffer( command_buffer );
        }
    }

    auto Image::get_descriptor_set( ) const -> VkDescriptorSet
    {
        return descriptor;
    }

    auto Image::get_width( ) const -> std::uint32_t
    {
        return width;
    }
    auto Image::get_height( ) const -> std::uint32_t
    {
        return height;
    }

    auto Image::allocate( std::size_t size ) -> void
    {
        VkDevice device = internal::get_device( );

        VkResult err;

        VkFormat vulkanFormat = to_vulkan_image_format( format );

        // Create the Image
        {
            VkImageCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = vulkanFormat;
            info.extent.width = width;
            info.extent.height = height;
            info.extent.depth = 1;
            info.mipLevels = 1;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            err = vkCreateImage( device, &info, nullptr, &image );
            check_vk_result( err );
            VkMemoryRequirements req;
            vkGetImageMemoryRequirements( device, image, &req );
            VkMemoryAllocateInfo alloc_info = { };
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = req.size;
            alloc_info.memoryTypeIndex = vulkan_memory_type( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits );
            err = vkAllocateMemory( device, &alloc_info, nullptr, &memory );
            check_vk_result( err );
            err = vkBindImageMemory( device, image, memory, 0 );
            check_vk_result( err );
        }

        // Create the Image View:
        {
            VkImageViewCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = image;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = vulkanFormat;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.layerCount = 1;
            err = vkCreateImageView( device, &info, nullptr, &image_view );
            check_vk_result( err );
        }

        // Create sampler:
        {
            VkSamplerCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            info.magFilter = VK_FILTER_LINEAR;
            info.minFilter = VK_FILTER_LINEAR;
            info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.minLod = -1000;
            info.maxLod = 1000;
            info.maxAnisotropy = 1.0f;
            VkResult err = vkCreateSampler( device, &info, nullptr, &sampler );
            check_vk_result( err );
        }

        // Create the Descriptor Set:
        descriptor = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture( sampler, image_view,
                                                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
    }

    auto Image::release( ) -> void
    {
        internal::submit_resource_free(
            [ s = sampler, iv = image_view, i = image, m = memory, b = buffer, dm = device_memory ]( ) {
                VkDevice device = internal::get_device( );

                vkDestroySampler( device, s, nullptr );
                vkDestroyImageView( device, iv, nullptr );
                vkDestroyImage( device, i, nullptr );
                vkFreeMemory( device, m, nullptr );
                vkDestroyBuffer( device, b, nullptr );
                vkFreeMemory( device, dm, nullptr );
            } );

        sampler = nullptr;
        image_view = nullptr;
        image = nullptr;
        memory = nullptr;
        buffer = nullptr;
        device_memory = nullptr;
    }
} // namespace vrock::ui
