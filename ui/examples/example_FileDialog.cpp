#include "imgui.h"
#include <future>
#include <iostream>
#include <memory>
#include <misc/cpp/imgui_stdlib.h>
#include <utility>

import vrock.ui.Widgets; // import order is important
import vrock.ui.Application;
import vrock.ui.KeyCode;
import vrock.utils.FutureHelper;
import vrock.ui.FileDialog;
#include <format>

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
        ImGui::DockSpaceOverViewport( ImGui::GetMainViewport( ) );

        // check if the file dialog has returned a result
        if ( open_file_fut.valid( ) && vrock::utils::future_ready( open_file_fut ) )
            // get the result from the future
            open_file_path = open_file_fut.get( );

        // check if the file dialog has returned a result
        if ( open_folder_fut.valid( ) && vrock::utils::future_ready( open_folder_fut ) )
            // get the result from the future
            open_folder_path = open_folder_fut.get( );

        // check if the file dialog has returned a result
        if ( save_file_fut.valid( ) && vrock::utils::future_ready( save_file_fut ) )
            // get the result from the future
            save_file_path = save_file_fut.get( );

        if ( ImGui::Begin( "App" ) )
        {
            if ( !open_file_path.empty( ) )
                ImGui::Text( "%s", std::format( "file path: {}", open_file_path ).c_str( ) );
            if ( ImGui::Button( "Open File" ) && !open_file_fut.valid( ) )
                // opening the systems file dialog is just this call (remove _async for synchronous call)
                open_file_fut = std::move( vrock::ui::open_file_async( "Open File", "", { } ) );
            ImGui::Separator( );

            if ( !open_folder_path.empty( ) )
                ImGui::Text( "%s", std::format( "folder path: {}", open_folder_path ).c_str( ) );
            if ( ImGui::Button( "Open Folder" ) && !open_folder_fut.valid( ) )
                // opening the systems file dialog is just this call (remove _async for synchronous call)
                open_folder_fut = std::move( vrock::ui::select_folder_async( "Open Folder", "" ) );

            ImGui::Separator( );

            if ( !save_file_path.empty( ) )
                ImGui::Text( "%s", std::format( "file save path: {}", save_file_path ).c_str( ) );
            if ( ImGui::Button( "Save File" ) && !save_file_fut.valid( ) )
                // opening the systems file dialog is just this call (remove _async for synchronous call)
                // FILE WILL NOT BE SAVED WITH THIS CALL! it just returns a file path to save to
                save_file_fut = std::move( vrock::ui::save_file_async( "Open Folder", "", { } ) );
            ImGui::End( );
        }
    }

    std::string open_file_path;
    std::future<std::string> open_file_fut;
    std::string open_folder_path;
    std::future<std::string> open_folder_fut;
    std::string save_file_path;
    std::future<std::string> save_file_fut;
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