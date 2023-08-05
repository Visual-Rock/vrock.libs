module;

#define IMGUI_ENABLE_FREETYPE
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <future>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

import vrock.utils.FutureHelper;

export module vrock.ui.Widgets;

namespace vrock::ui
{
    class Application;
    export class Dialog;
    export template <class T>
    class ModalDialog;

    export class BaseWidget
    {
    public:
        BaseWidget( std::shared_ptr<vrock::ui::Application> app ) : app( app )
        {
        }

        ~BaseWidget( );

        auto on_update( ) -> void;
        auto on_render( ) -> void;
        auto on_after_render( ) -> void;
        auto on_terminate( ) -> void;

        virtual void setup( )
        {
        }
        virtual void update( )
        {
        }
        virtual void render( )
        {
        }
        virtual void after_render( )
        {
        }
        virtual void terminate( )
        {
        }

        auto get_visibility( ) const -> bool;
        auto set_visibility( bool new_visibility ) -> void;

        template <class T, class... Args>
            requires std::is_base_of_v<BaseWidget, T>
        auto create_child( Args &&...args ) -> std::shared_ptr<T>;

    protected:
        bool visibility = true;

        std::shared_ptr<vrock::ui::Application> app;
        std::vector<std::shared_ptr<BaseWidget>> children;
    };

    class Dialog : public BaseWidget
    {
    public:
        Dialog( std::shared_ptr<vrock::ui::Application> app, std::string title,
                ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking )
            : BaseWidget( std::move( app ) ), title( std::move( title ) ), closed( false ),
              flags( flags | ImGuiWindowFlags_Popup )
        {
        }

        ~Dialog( ) = default;

        auto close( ) noexcept -> void
        {
            closed = true;
        }

        [[nodiscard]] auto is_closed( ) const noexcept -> bool
        {
            return closed;
        }

        [[nodiscard]] auto get_title( ) const noexcept -> std::string
        {
            return title;
        }

        [[nodiscard]] auto get_flags( ) const noexcept -> ImGuiWindowFlags
        {
            return flags;
        }

    private:
        std::string title;
        bool closed = false;
        ImGuiWindowFlags flags;
    };

    template <class T>
    class ModalDialog : public Dialog
    {
    public:
        ModalDialog( std::shared_ptr<vrock::ui::Application> app, std::string title,
                     ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking )
            : Dialog( app, title, flags )
        {
        }
        ~ModalDialog( )
        {
            if ( !utils::future_ready( future ) )
                result.set_value( T( ) );
        }

        auto close( T res = T( ) ) -> void
        {
            result.set_value( res );
            Dialog::close( );
        }

        auto set_promise( std::promise<T> promise ) -> void
        {
            result = std::move( promise );
            future = result.get_future( ).share( );
        }

        auto get_future( ) -> std::shared_future<T>
        {
            return future;
        }

    private:
        std::promise<T> result;
        std::shared_future<T> future;
    };
} // namespace vrock::ui