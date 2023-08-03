#include "imgui.h"
#include <iostream>
#include <memory>
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

    void setup( ) final
    {
    }
    void render( ) final
    {
        ImGui::ShowDemoWindow( );
    }
    void terminate( ) final
    {
    }
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