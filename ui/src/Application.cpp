module;

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "imgui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

import vrock.utils.Timer;
export import vrock.ui.Widgets;
import vrock.utils.FutureHelper;
export import vrock.ui.KeyCode;

#define SPECTRUM_USE_DARK_THEME
#include "./imgui_theme/spectrum.h"

#include "font.hpp"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <future>

module vrock.ui.Application;

namespace vrock::ui::internal
{
    void check_vk_result( VkResult err )
    {
        if ( err == 0 )
            return;
        fprintf( stderr, "[vulkan] Error: VkResult = %d\n", err );
        if ( err < 0 )
            abort( );
    }

    auto get_command_buffer( ) -> VkCommandBuffer
    {
        ImGui_ImplVulkanH_Window *wd = &main_window_data;

        // Use any command queue
        VkCommandPool command_pool = wd->Frames[ wd->FrameIndex ].CommandPool;

        VkCommandBufferAllocateInfo cmdBufAllocateInfo = { };
        cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufAllocateInfo.commandPool = command_pool;
        cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufAllocateInfo.commandBufferCount = 1;

        VkCommandBuffer &command_buffer = allocated_command_buffers[ wd->FrameIndex ].emplace_back( );
        auto err = vkAllocateCommandBuffers( device, &cmdBufAllocateInfo, &command_buffer );

        VkCommandBufferBeginInfo begin_info = { };
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer( command_buffer, &begin_info );
        check_vk_result( err );

        return command_buffer;
    }

    auto flush_command_buffer( VkCommandBuffer buff ) -> void
    {
        const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

        VkSubmitInfo end_info = { };
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &buff;
        auto err = vkEndCommandBuffer( buff );
        check_vk_result( err );

        // Create fence to ensure that the command buffer has finished executing
        VkFenceCreateInfo fenceCreateInfo = { };
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = 0;
        VkFence fence;
        err = vkCreateFence( device, &fenceCreateInfo, nullptr, &fence );
        check_vk_result( err );

        err = vkQueueSubmit( queue, 1, &end_info, fence );
        check_vk_result( err );

        err = vkWaitForFences( device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT );
        check_vk_result( err );

        vkDestroyFence( device, fence, nullptr );
    }
} // namespace vrock::ui::internal

using namespace vrock::ui::internal;

void glfw_error_callback( int error, const char *description )
{
    // vrock::log::get_logger( "ui" ).error( "GLFW Error {}: {}", error, description );
}

void SetupVulkan( const char **extensions, uint32_t extensions_count )
{
    VkResult err;

    // Create Vulkan Instance
    {
        VkInstanceCreateInfo create_info = { };
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.enabledExtensionCount = extensions_count;
        create_info.ppEnabledExtensionNames = extensions;
#ifdef IMGUI_VULKAN_DEBUG_REPORT
        // Enabling validation layers
        const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = layers;

        // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new
        // extension to it)
        const char **extensions_ext = (const char **)malloc( sizeof( const char * ) * ( extensions_count + 1 ) );
        memcpy( extensions_ext, extensions, extensions_count * sizeof( const char * ) );
        extensions_ext[ extensions_count ] = "VK_EXT_debug_report";
        create_info.enabledExtensionCount = extensions_count + 1;
        create_info.ppEnabledExtensionNames = extensions_ext;

        // Create Vulkan Instance
        err = vkCreateInstance( &create_info, allocator, &instance );
        check_vk_result( err );
        free( extensions_ext );

        // Get the function pointer (required for any extensions)
        auto vkCreateDebugReportCallbackEXT =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );
        IM_ASSERT( vkCreateDebugReportCallbackEXT != NULL );

        // Setup the debug report callback
        VkDebugReportCallbackCreateInfoEXT debug_report_ci = { };
        debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = debug_report;
        debug_report_ci.pUserData = NULL;
        err = vkCreateDebugReportCallbackEXT( instance, &debug_report_ci, allocator, &debug_report );
        check_vk_result( err );
#else
        // Create Vulkan Instance without any debug feature
        err = vkCreateInstance( &create_info, allocator, &instance );
        check_vk_result( err );
        IM_UNUSED( debug_report );
#endif
    }

    // Select GPU
    {
        uint32_t gpu_count;
        err = vkEnumeratePhysicalDevices( instance, &gpu_count, NULL );
        check_vk_result( err );
        IM_ASSERT( gpu_count > 0 );

        VkPhysicalDevice *gpus = (VkPhysicalDevice *)malloc( sizeof( VkPhysicalDevice ) * gpu_count );
        err = vkEnumeratePhysicalDevices( instance, &gpu_count, gpus );
        check_vk_result( err );

        // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
        // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
        // dedicated GPUs) is out of scope of this sample.
        int use_gpu = 0;
        for ( int i = 0; i < (int)gpu_count; i++ )
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties( gpus[ i ], &properties );
            if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
            {
                use_gpu = i;
                break;
            }
        }

        physical_device = gpus[ use_gpu ];
        free( gpus );
    }

    // Select graphics queue family
    {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &count, NULL );
        VkQueueFamilyProperties *queues =
            (VkQueueFamilyProperties *)malloc( sizeof( VkQueueFamilyProperties ) * count );
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &count, queues );
        for ( uint32_t i = 0; i < count; i++ )
            if ( queues[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
            {
                queue_family = i;
                break;
            }
        free( queues );
        IM_ASSERT( queue_family != (uint32_t)-1 );
    }

    // Create Logical Device (with 1 queue)
    {
        int device_extension_count = 1;
        const char *device_extensions[] = { "VK_KHR_swapchain" };
        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queue_info[ 1 ] = { };
        queue_info[ 0 ].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[ 0 ].queueFamilyIndex = queue_family;
        queue_info[ 0 ].queueCount = 1;
        queue_info[ 0 ].pQueuePriorities = queue_priority;
        VkDeviceCreateInfo create_info = { };
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = sizeof( queue_info ) / sizeof( queue_info[ 0 ] );
        create_info.pQueueCreateInfos = queue_info;
        create_info.enabledExtensionCount = device_extension_count;
        create_info.ppEnabledExtensionNames = device_extensions;
        err = vkCreateDevice( physical_device, &create_info, allocator, &device );
        check_vk_result( err );
        vkGetDeviceQueue( device, queue_family, 0, &queue );
    }

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                                              { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                                              { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };
        VkDescriptorPoolCreateInfo pool_info = { };
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE( pool_sizes );
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE( pool_sizes );
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool( device, &pool_info, allocator, &descriptor_pool );
        check_vk_result( err );
    }
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL debug_report( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                             uint64_t object, size_t location, int32_t messageCode,
                                             const char *pLayerPrefix, const char *pMessage, void *pUserData )
{
    (void)flags;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix; // Unused arguments
    fprintf( stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage );
    return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void SetupVulkanWindow( ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height )
{
    wd->Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR( physical_device, queue_family, wd->Surface, &res );
    if ( res != VK_TRUE )
    {
        fprintf( stderr, "Error no WSI support on physical device 0\n" );
        exit( -1 );
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
                                                   VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat( physical_device, wd->Surface, requestSurfaceImageFormat,
                                                               (size_t)IM_ARRAYSIZE( requestSurfaceImageFormat ),
                                                               requestSurfaceColorSpace );

    // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
                                         VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode( physical_device, wd->Surface, &present_modes[ 0 ],
                                                           IM_ARRAYSIZE( present_modes ) );
    // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT( min_image_count >= 2 );
    ImGui_ImplVulkanH_CreateOrResizeWindow( instance, physical_device, device, wd, queue_family, allocator, width,
                                            height, min_image_count );
}

void CleanupVulkan( )
{
    vkDestroyDescriptorPool( device, descriptor_pool, allocator );

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    // Remove the debug report callback
    auto vkDestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" );
    vkDestroyDebugReportCallbackEXT( instance, debug_report, allocator );
#endif // IMGUI_VULKAN_DEBUG_REPORT

    vkDestroyDevice( device, allocator );
    vkDestroyInstance( instance, allocator );
}

void CleanupVulkanWindow( )
{
    ImGui_ImplVulkanH_DestroyWindow( instance, device, &main_window_data, allocator );
}

void FrameRender( ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data )
{
    VkResult err;

    VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR( device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE,
                                 &wd->FrameIndex );
    if ( err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR )
    {
        swap_chain_rebuild = true;
        return;
    }
    check_vk_result( err );

    current_frame_index = ( current_frame_index + 1 ) % main_window_data.ImageCount;

    ImGui_ImplVulkanH_Frame *fd = &wd->Frames[ wd->FrameIndex ];
    {
        err = vkWaitForFences( device, 1, &fd->Fence, VK_TRUE,
                               UINT64_MAX ); // wait indefinitely instead of periodically checking
        check_vk_result( err );

        err = vkResetFences( device, 1, &fd->Fence );
        check_vk_result( err );
    }

    {
        // Free resources in queue
        for ( auto &func : resource_free_queue[ current_frame_index ] )
            func( );
        resource_free_queue[ current_frame_index ].clear( );
    }
    {
        // Free command buffers allocated by Application::GetCommandBuffer
        // These use main_window_data.FrameIndex and not current_frame_index because they're tied to the swapchain image
        // index
        auto &allocatedCommandBuffers = allocated_command_buffers[ wd->FrameIndex ];
        if ( allocatedCommandBuffers.size( ) > 0 )
        {
            vkFreeCommandBuffers( device, fd->CommandPool, (uint32_t)allocatedCommandBuffers.size( ),
                                  allocatedCommandBuffers.data( ) );
            allocatedCommandBuffers.clear( );
        }

        err = vkResetCommandPool( device, fd->CommandPool, 0 );
        check_vk_result( err );
        VkCommandBufferBeginInfo info = { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer( fd->CommandBuffer, &info );
        check_vk_result( err );
    }
    {
        VkRenderPassBeginInfo info = { };
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = wd->RenderPass;
        info.framebuffer = fd->Framebuffer;
        info.renderArea.extent.width = wd->Width;
        info.renderArea.extent.height = wd->Height;
        info.clearValueCount = 1;
        info.pClearValues = &wd->ClearValue;
        vkCmdBeginRenderPass( fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE );
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData( draw_data, fd->CommandBuffer );

    // Submit command buffer
    vkCmdEndRenderPass( fd->CommandBuffer );
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = { };
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->CommandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;

        err = vkEndCommandBuffer( fd->CommandBuffer );
        check_vk_result( err );
        err = vkQueueSubmit( queue, 1, &info, fd->Fence );
        check_vk_result( err );
    }
}

void FramePresent( ImGui_ImplVulkanH_Window *wd )
{
    if ( swap_chain_rebuild )
        return;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].RenderCompleteSemaphore;
    VkPresentInfoKHR info = { };
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &wd->FrameIndex;
    VkResult err = vkQueuePresentKHR( queue, &info );
    if ( err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR )
    {
        swap_chain_rebuild = true;
        return;
    }
    check_vk_result( err );
    wd->SemaphoreIndex = ( wd->SemaphoreIndex + 1 ) % wd->ImageCount; // Now we can use the next set of semaphores
}

namespace vrock::ui
{

}
