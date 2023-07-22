module;

#include <functional>
#include <future>
#include <memory>
#include <string>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

export import vrock.ui.Widgets;

export module vrock.ui.Application;

namespace vrock::ui
{
    export class ApplicationConfig
    {
    public:
        std::string application_name = "vrock.ui";
        uint32_t width = 1280;
        uint32_t height = 720;

        ImGuiConfigFlags config_flags = 0;
        ImGuiBackendFlags backend_flags = 0;
    };

    class Application
    {
    public:
        Application( )
        {
        }
        ~Application( )
        {
        }

        auto run( const ApplicationConfig &config, std::shared_ptr<BaseWidget> root ) -> int;

        template <class T>
            requires std::is_base_of_v<Dialog, T>
        auto open_modal_dialog( std::shared_ptr<ModalDialog<T>> dialog ) -> std::shared_future<T>
        {
            std::promise<T> p;
            dialog->set_promise( std::move( p ) );
            dialogs.push_back( dialog );
            new_dialogs.push_back( dialog );
            return dialog->get_future( );
        }

        // auto key_down( KeyCode ) -> bool;

        // auto mouse_button_pressed( MouseButton ) -> bool;
        // auto mouse_position( ) -> std::pair<double, double>;

        // auto cursor_mode( CursorMode ) -> void;

        auto rename_window( const std::string &title ) -> void;
        auto close_handler( std::function<bool( )> fn ) -> void;

        /// @brief in seconds
        [[nodiscard]] auto inline get_delta( ) const -> float
        {
            return delta;
        }

    protected:
        float delta = 0;

        std::function<void( const std::string & )> rename;
        std::function<bool( )> close_handler_ = []( ) { return true; };
        bool should_close = false;
        std::vector<std::shared_ptr<Dialog>> dialogs = { };
        std::vector<std::shared_ptr<Dialog>> new_dialogs = { };
    };

    export namespace internal
    {
        void check_vk_result( VkResult err );

        auto get_command_buffer( bool start ) -> VkCommandBuffer;

        auto flush_command_buffer( VkCommandBuffer buff ) -> void;

        auto submit_resource_free( std::function<void( )> &&func ) -> void;

        auto get_window( ) -> GLFWwindow *;

        auto get_device( ) -> VkDevice;

        auto get_physical_device( ) -> VkPhysicalDevice;
    } // namespace internal
} // namespace vrock::ui