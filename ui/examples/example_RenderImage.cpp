#include "imgui.h"
#include <iostream>
#include <memory>
#include <utility>

import vrock.ui.Widgets; // import order is important
import vrock.ui.Application;
import vrock.ui.Image;
import vrock.ui.KeyCode;

using BaseWidget = vrock::ui::BaseWidget;
using ApplicationConfig = vrock::ui::ApplicationConfig;
using Application = vrock::ui::Application;
using Image = vrock::ui::Image;

class MainWindow : public BaseWidget
{
public:
    explicit MainWindow( std::shared_ptr<vrock::ui::Application> app ) : BaseWidget( std::move( app ) )
    {
    }

    void setup( ) final
    {
        // load image from disk
        img = std::make_unique<Image>( "image.png" );
    }
    void render( ) final
    {
        ImGui::Begin( "Image" );
        if ( img )
            // first we supply the image id and after that the image size (the image
            // can be scaled up or down here by multiplying the width and the height)
            ImGui::Image( img->get_descriptor_set( ), { (float)img->get_width( ) / 2, (float)img->get_height( ) / 2 } );

        ImGui::End( );
    }
    void terminate( ) final
    {
    }

    std::unique_ptr<Image> img;
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