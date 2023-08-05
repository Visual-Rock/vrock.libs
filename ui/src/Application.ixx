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
#include <stack>

export module vrock.ui.Application;

inline GLFWwindow *window = nullptr;

inline VkAllocationCallbacks *allocator = NULL;
inline VkInstance instance = VK_NULL_HANDLE;
inline VkPhysicalDevice physical_device = VK_NULL_HANDLE;
inline VkDevice device = VK_NULL_HANDLE;
inline uint32_t queue_family = (uint32_t)-1;
inline VkQueue queue = VK_NULL_HANDLE;
inline VkDebugReportCallbackEXT debug_report = VK_NULL_HANDLE;
inline VkPipelineCache pipeline_cache = VK_NULL_HANDLE;
inline VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

inline ImGui_ImplVulkanH_Window main_window_data;
inline int min_image_count = 2;
inline bool swap_chain_rebuild = false;

// Per-frame-in-flight
inline std::vector<std::vector<VkCommandBuffer>> allocated_command_buffers;
inline std::vector<std::vector<std::function<void( )>>> resource_free_queue;

// Unlike main_window_data.FrameIndex, this is not the the swapchain image index
// and is always guaranteed to increase (eg. 0, 1, 2, 0, 1, 2)
inline uint32_t current_frame_index = 0;

export namespace vrock::ui::internal
{
    /**
     * @brief Checks the result of a Vulkan operation.
     * @param err The result of a Vulkan operation of type VkResult.
     */
    void check_vk_result( VkResult err );

    /**
     * @brief Retrieves a command buffer.
     * @return Vulkan Command Buffer.
     */
    auto get_command_buffer( ) -> VkCommandBuffer;

    /**
     * @brief Flushes a command buffer.
     * @param buff Vulkan Command Buffer that needs to be flushed.
     */
    auto flush_command_buffer( VkCommandBuffer buff ) -> void;

    /**
     * @brief Submits a resource free function to the queue.
     * @param func A function that frees a resource.
     */
    auto submit_resource_free( std::function<void( )> &&func ) -> void
    {
        resource_free_queue[ current_frame_index ].emplace_back( func );
    }

    /**
     * @brief Retrieves the GLFW window.
     * @return Pointer to the current GLFW window.
     */
    auto get_window( ) -> GLFWwindow *
    {
        return window;
    }

    /**
     * @brief Retrieves the Vulkan device.
     * @return The current Vulkan device.
     */
    auto get_device( ) -> VkDevice
    {
        return device;
    }

    /**
     * @brief Retrieves the physical Vulkan device.
     * @return The current physical Vulkan device.
     */
    auto get_physical_device( ) -> VkPhysicalDevice
    {
        return physical_device;
    }
} // namespace vrock::ui::internal

using namespace vrock::ui::internal;

void glfw_error_callback( int error, const char *description );

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
void SetupVulkan( const char **extensions, uint32_t extensions_count );
void SetupVulkanWindow( ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height );
void CleanupVulkan( );
void CleanupVulkanWindow( );
void FrameRender( ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data );
void FramePresent( ImGui_ImplVulkanH_Window *wd );

namespace vrock::ui
{
    /**
     * @class ApplicationConfig
     * @brief Represents the configuration of an application.
     */
    export class ApplicationConfig
    {
    public:
        /**
         * @brief The name of the application.
         * This gets displayed in the application window title.
         * Default value is "vrock.ui".
         */
        std::string application_name = "vrock.ui";

        /**
         * @brief The width of the application window.
         * Default value is 1280 pixels.
         */
        uint32_t width = 1280;

        /**
         * @brief The height of the application window.
         * Default value is 720 pixels.
         */
        uint32_t height = 720;

        /**
         * @brief Flags for ImGui configuration.
         * This controls some high-level ImGui behaviors. Flags can be combined using the bitwise OR operation.
         * Default value is 0.
         */
        ImGuiConfigFlags config_flags = 0;

        /**
         * @brief Flags for ImGui backend.
         * Default value is 0.
         */
        ImGuiBackendFlags backend_flags = 0;
    };

    /**
     * @class Application
     * @brief Represents the application as a whole, and is responsible for initializing, running the main loop, and
     * terminating the app.
     */
    export class Application
    {
    public:
        /**
         * @brief Default constructor
         */
        Application( ) = default;

        /**
         * @brief Default destructor
         */
        ~Application( ) = default;

        /**
         * @brief Runs the application.
         * @param config The configuration settings (object of type `ApplicationConfig`) to run the application
         * @param root A shared pointer to the root widget (object of type BaseWidget).
         * @return integer value indicating the status of the operation
         */
        auto run( const ApplicationConfig &config, std::shared_ptr<BaseWidget> root ) -> int
        {
            // logger.debug( "initializing Window" );
            glfwSetErrorCallback( glfw_error_callback );

            /* Initialize the library */
            if ( !glfwInit( ) )
                return -1;

            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

            // logger.debug( "creating Window" );
            /* Create a windowed mode window and its OpenGL context */
            window = glfwCreateWindow( (int)config.width, (int)config.height, config.application_name.c_str( ), nullptr,
                                       nullptr );
            if ( !window )
            {
                glfwTerminate( );
                return -1;
            }

            if ( !glfwVulkanSupported( ) )
            {
                // log::get_logger( "ui" ).error( "Vulkan not supported!" );
                return -1;
            }

            glfwSetInputMode( window, GLFW_STICKY_KEYS, GLFW_TRUE );

            uint32_t extensions_count = 0;
            const char **extensions = glfwGetRequiredInstanceExtensions( &extensions_count );
            SetupVulkan( extensions, extensions_count );

            VkSurfaceKHR surface;
            VkResult err = glfwCreateWindowSurface( instance, window, allocator, &surface );
            check_vk_result( err );

            int w, h;
            glfwGetFramebufferSize( window, &w, &h );
            ImGui_ImplVulkanH_Window *wd = &main_window_data;
            SetupVulkanWindow( wd, surface, w, h );

            allocated_command_buffers.resize( wd->ImageCount );
            resource_free_queue.resize( wd->ImageCount );

            // logger.debug( "initializing ImGui" );

            rename = [ & ]( const std::string &title ) { glfwSetWindowTitle( window, title.c_str( ) ); };

            IMGUI_CHECKVERSION( );
            ImGui::CreateContext( );
            ImGuiIO &io = ImGui::GetIO( );
            io.ConfigFlags = config.config_flags;
            io.BackendFlags = config.backend_flags;

            // Setup Platform / Renderer backends
            ImGui_ImplGlfw_InitForVulkan( window, true );
            ImGui_ImplVulkan_InitInfo init_info = { };
            init_info.Instance = instance;
            init_info.PhysicalDevice = physical_device;
            init_info.Device = device;
            init_info.QueueFamily = queue_family;
            init_info.Queue = queue;
            init_info.PipelineCache = pipeline_cache;
            init_info.DescriptorPool = descriptor_pool;
            init_info.Subpass = 0;
            init_info.MinImageCount = min_image_count;
            init_info.ImageCount = wd->ImageCount;
            init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            init_info.Allocator = allocator;
            init_info.CheckVkResultFn = check_vk_result;
            ImGui_ImplVulkan_Init( &init_info, wd->RenderPass );

            ImGui::Spectrum::StyleColorsSpectrum( );

            ImFont *font =
                io.Fonts->AddFontFromMemoryCompressedTTF( roboto_compressed_data, roboto_compressed_size, 18.0 );
            if ( font )
                io.FontDefault = font;
            {
                // Use any command queue
                VkCommandPool command_pool = wd->Frames[ wd->FrameIndex ].CommandPool;
                VkCommandBuffer command_buffer = wd->Frames[ wd->FrameIndex ].CommandBuffer;

                err = vkResetCommandPool( device, command_pool, 0 );
                check_vk_result( err );
                VkCommandBufferBeginInfo begin_info = { };
                begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                err = vkBeginCommandBuffer( command_buffer, &begin_info );
                check_vk_result( err );

                ImGui_ImplVulkan_CreateFontsTexture( command_buffer );

                VkSubmitInfo end_info = { };
                end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                end_info.commandBufferCount = 1;
                end_info.pCommandBuffers = &command_buffer;
                err = vkEndCommandBuffer( command_buffer );
                check_vk_result( err );
                err = vkQueueSubmit( queue, 1, &end_info, VK_NULL_HANDLE );
                check_vk_result( err );

                err = vkDeviceWaitIdle( device );
                check_vk_result( err );
                ImGui_ImplVulkan_DestroyFontUploadObjects( );
            }

            // logger.debug( "initializing Main Window" );
            root->setup( );

            std::unique_ptr<std::future<bool>> close = nullptr;

            utils::Timer timer;

            while ( !should_close )
            {
                timer.reset( );
                root->on_update( );
                for ( auto &d : dialogs )
                    d->on_update( );
                // close handling
                if ( glfwWindowShouldClose( window ) && close == nullptr )
                {
                    close = std::make_unique<std::future<bool>>( std::async( close_handler_ ) );
                    glfwSetWindowShouldClose( window, GLFW_FALSE );
                }
                if ( close != nullptr && close->valid( ) )
                {
                    if ( close->wait_for( std::chrono::milliseconds( 0 ) ) == std::future_status::ready )
                    {
                        should_close = close->get( );
                        close = nullptr;
                    }
                }

                glfwPollEvents( );
                // Resize swap chain?
                if ( swap_chain_rebuild )
                {
                    int width, height;
                    glfwGetFramebufferSize( window, &width, &height );
                    if ( width > 0 && height > 0 )
                    {
                        ImGui_ImplVulkan_SetMinImageCount( min_image_count );
                        ImGui_ImplVulkanH_CreateOrResizeWindow( instance, physical_device, device, &main_window_data,
                                                                queue_family, allocator, width, height,
                                                                min_image_count );
                        main_window_data.FrameIndex = 0;

                        // Clear allocated command buffers from here since entire pool is destroyed
                        allocated_command_buffers.clear( );
                        allocated_command_buffers.resize( main_window_data.ImageCount );

                        swap_chain_rebuild = false;
                    }
                }

                // Start the Dear ImGui frame
                ImGui_ImplVulkan_NewFrame( );
                ImGui_ImplGlfw_NewFrame( );

                ImGui::NewFrame( );

                root->on_render( );

                render_modal( 0 );
                if ( !new_dialogs.empty( ) )
                    new_dialogs.clear( );
                if ( !dialogs.empty( ) && dialogs.back( )->is_closed( ) )
                    dialogs.pop_back( );

                ImGui::Render( );

                ImDrawData *main_draw_data = ImGui::GetDrawData( );
                const bool main_is_minimized =
                    ( main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f );
                if ( !main_is_minimized )
                    FrameRender( wd, main_draw_data );

                // Update and Render additional Platform Windows
                if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
                {
                    ImGui::UpdatePlatformWindows( );
                    ImGui::RenderPlatformWindowsDefault( );
                }

                // Present Main Platform Window
                if ( !main_is_minimized )
                    FramePresent( wd );

                delta = (float)timer.elapsed<std::chrono::nanoseconds>( ) * 0.000000001f;

                root->on_after_render( );
                for ( auto &d : dialogs )
                    d->on_after_render( );
            }
            // logger.debug( "cleanup" );
            rename = [ & ]( const std::string &title ) { /*logger.debug( "can't rename window after cleanup!" );*/ };
            root->on_terminate( );

            // Cleanup
            err = vkDeviceWaitIdle( device );
            check_vk_result( err );

            // Free resources in queue
            for ( auto &queue : resource_free_queue )
            {
                for ( auto &func : queue )
                    func( );
            }
            resource_free_queue.clear( );

            ImGui_ImplVulkan_Shutdown( );
            ImGui_ImplGlfw_Shutdown( );
            ImGui::DestroyContext( );

            CleanupVulkanWindow( );
            CleanupVulkan( );

            glfwDestroyWindow( window );
            glfwTerminate( );
            return 0;
        }

        /**
         * @brief Opens a modal dialog.
         *
         * This function opens up a Modal Dialog using the supplied shared_ptr.
         *
         * @tparam T The type of the value expected to be returned by the Modal Dialog.
         *           This type must fulfill the DefaultConstructible requirement.
         *
         * @param dialog An std::shared_ptr to a ModalDialog.
         * @return An std::shared_future that will contain the return value from the dialog.
         */
        template <class T>
            requires std::is_default_constructible_v<T>
        auto open_modal_dialog( std::shared_ptr<ModalDialog<T>> dialog ) -> std::shared_future<T>
        {
            std::promise<T> p;
            dialog->set_promise( std::move( p ) );
            dialogs.push_back( dialog );
            new_dialogs.push_back( dialog );
            return dialog->get_future( );
        }

        auto key_down( KeyCode key ) -> bool
        {
            int status = glfwGetKey( window, (int)key );
            return status == GLFW_PRESS || status == GLFW_REPEAT;
        }

        /**
         * @brief Check if a mouse button is pressed.
         * @param button The button to check.
         * @return bool value indicating the status of the button.
         */
        auto mouse_button_pressed( MouseButton button ) -> bool
        {
            return glfwGetMouseButton( window, (int)button ) == GLFW_PRESS;
        }

        /**
         * @brief Get the current position of the mouse cursor.
         * @return A pair of double values indicating the x and y coordinates of the mouse cursor.
         */
        auto mouse_position( ) -> std::pair<double, double>
        {
            double x, y;
            glfwGetCursorPos( window, &x, &y );
            return std::make_pair( x, y );
        }

        /**
         * @brief Sets the mode of the cursor.
         * @param mode The mode to set the cursor to.
         */
        auto cursor_mode( CursorMode mode ) -> void
        {
            glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode );
        }

        /**
         * @brief Renames the application window title.
         * @param title The new title name.
         */
        auto rename_window( const std::string &title ) -> void
        {
            rename( title );
        }

        /**
         * @brief Defines the close handling behavior of the application.
         * @param fn A function object that is invoked when the app is asked to close.
         */
        auto close_handler( std::function<bool( )> fn ) -> void
        {
            close_handler_ = std::move( fn );
        }

        /**
         * @return The amount of time that has passed since the last frame update. (in seconds)
         */
        [[nodiscard]] auto inline get_delta( ) const -> float
        {
            return delta;
        }

    protected:
        auto render_modal( std::size_t offset ) -> void
        {
            if ( offset == dialogs.size( ) )
                return;

            for ( auto &nd : new_dialogs )
            {
                if ( dialogs[ offset ] == nd )
                {
                    nd->setup( );
                    ImGui::OpenPopup( nd->get_title( ).c_str( ), nd->get_flags( ) );
                }
            }

            bool t = true;
            if ( ImGui::BeginPopupModal( dialogs[ offset ]->get_title( ).c_str( ), &t,
                                         dialogs[ offset ]->get_flags( ) ) )
            {
                dialogs[ offset ]->on_render( );
                render_modal( ++offset );
                ImGui::EndPopup( );
            }
            else
                dialogs[ offset ]->close( );
        }

        float delta = 0;

        std::function<void( const std::string & )> rename;
        std::function<bool( )> close_handler_ = []( ) { return true; };
        bool should_close = false;
        std::deque<std::shared_ptr<Dialog>> dialogs = { };
        std::vector<std::shared_ptr<Dialog>> new_dialogs = { };
    };
} // namespace vrock::ui
