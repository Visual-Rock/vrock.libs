#include <chrono>
#include <future>
#include <imgui.h>
#include <memory>

import vrock.ui.Widgets;
import vrock.utils.FutureHelper;
import vrock.ui.Application;

class DialogClose : public vrock::ui::ModalDialog<bool>
{
public:
    DialogClose( std::shared_ptr<vrock::ui::Application> app, std::string title )
        : vrock::ui::ModalDialog<bool>( app, std::move( title ),
                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking )
    {
    }

    void setup( ) final
    {
    }

    void render( ) final
    {
        ImGui::Text( "Are you sure you want to close the App?" );
        if ( ImGui::Button( "close" ) )
            close( true );
        ImGui::SameLine( );
        if ( ImGui::Button( "resume" ) )
            close( false );
    }

    void terminate( ) final
    {
    }
};

class MainWindow : public vrock::ui::BaseWidget
{
public:
    MainWindow( std::shared_ptr<vrock::ui::Application> app ) : vrock::ui::BaseWidget( app )
    {
    }

    void setup( ) final
    {
        // Callback for the close button Event
        app->close_handler( [ & ]( ) {
            // open the Dialog
            auto res = app->open_modal_dialog<bool>( std::make_shared<DialogClose>( app, "Alert" ) );
            // wait for response
            while ( vrock::utils::future_ready( res, std::chrono::milliseconds( 10 ) ) )
            {
            }
            // return User choice
            return res.get( );
        } );
    }

    void render( ) final
    {
        // render demo window
        ImGui::ShowDemoWindow( );
    }

    void terminate( ) final
    {
    }
};

int main( )
{
    // auto log_cfg = vrock::log::LoggerConfig( "ui" ).set_log_level( vrock::log::Debug ).add_console_colored( );

    auto app_config = vrock::ui::ApplicationConfig( );
    app_config.application_name = "simple example";

    auto app = std::make_shared<vrock::ui::Application>( /*vrock::log::create_logger( log_cfg )*/ );
    app->run( app_config, std::make_shared<MainWindow>( app ) );
    return 0;
}