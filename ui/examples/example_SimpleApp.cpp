#include "imgui.h"
#include <iostream>
#include <memory>
#include <misc/cpp/imgui_stdlib.h>
#include <utility>

import vrock.ui.Widgets; // import order is important
import vrock.ui.Application;
import vrock.ui.KeyCode;

using BaseWidget = vrock::ui::BaseWidget;
using ApplicationConfig = vrock::ui::ApplicationConfig;
using Application = vrock::ui::Application;

class MainWindow : public BaseWidget
{
public:
    explicit MainWindow( std::shared_ptr<vrock::ui::Application> app ) : BaseWidget( std::move( app ) )
    {
    }

    void render( ) final
    {
        ImGui::ShowDemoWindow( );
        if ( ImGui::Begin( "App" ) )
        {
            ImGui::InputText( "title", &new_title );
            if ( ImGui::Button( "Rename" ) )
                // renaming the window is as easy as this little call to rename_window
                app->rename_window( new_title );
            ImGui::End( );
        }
    }

    std::string new_title;
};

int main( )
{
    // auto log_cfg = vrock::log::LoggerConfig( "ui" ).set_log_level( vrock::log::Debug ).add_console_colored( );

    auto app_config = ApplicationConfig( );
    app_config.application_name = "simple example";
    app_config.config_flags |= ImGuiConfigFlags_DockingEnable;

    auto app = std::make_shared<Application>( );
    app->run( app_config, std::make_shared<MainWindow>( app ) );
    return 0;
}