#include <imgui.h>

import vrock.ui.Application;
import vrock.ui.FileDialog;
import vrock.pdf.PDFDocument;

import Globals;
import DocumentStructureWidget;
import DocumentViewWidget;

#include <memory>
#include <string>

class MainWindow : public vrock::ui::BaseWidget
{
public:
    explicit MainWindow( std::shared_ptr<vrock::ui::Application> app ) : vrock::ui::BaseWidget( app )
    {
    }
    ~MainWindow( ) = default;

    void setup( ) final
    {
        structure = create_child<DocumentStructure>( app );
        view = create_child<DocumentView>( app );
    }
    void render( ) final
    {
        ImGui::DockSpaceOverViewport( ImGui::GetMainViewport( ) );

        if ( ImGui::BeginMainMenuBar( ) )
        {
            if ( ImGui::BeginMenu( "file" ) )
            {
                bool load = false;
                if ( ImGui::MenuItem( "load", "", &load ) )
                {
                    auto file = vrock::ui::open_file( "Select PDF Document", "", globals::pdf_file_filter );
                    globals::documents.push_back( std::make_shared<vrock::pdf::PDFDocument>( file ) );
                    globals::selected_document = globals::documents.size( ) - 1;
                }
                ImGui::EndMenu( );
            }

            view->render( );
            structure->on_render( );

            ImGui::EndMainMenuBar( );
        }
        ImGui::ShowMetricsWindow( &visibility );
    }

    std::shared_ptr<DocumentStructure> structure;
    std::shared_ptr<DocumentView> view;
};

int main( )
{
    auto app_config = vrock::ui::ApplicationConfig( );
    app_config.application_name = "Pdf Example";
    app_config.config_flags |= ImGuiConfigFlags_DockingEnable;

    auto app = std::make_shared<vrock::ui::Application>( /*logger*/ );
    app->run( app_config, std::make_shared<MainWindow>( app ) );
}
