module;

#include <imgui.h>

import vrock.ui.Application;
import vrock.utils.ByteArray;
import vrock.utils.Timer;
import vrock.ui.Image;
import vrock.pdf.PDFPageTree;
import vrock.pdf.RenderableObject;

import Globals;

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stb_image_resize.h>

export module DocumentViewWidget;

auto render_image_to_bytearray( std::shared_ptr<vrock::utils::ByteArray<>> array,
                                std::shared_ptr<vrock::pdf::Image> image, std::int32_t page_width,
                                std::int32_t page_height ) -> void;

export class DocumentView : public vrock::ui::BaseWidget
{
public:
    DocumentView( std::shared_ptr<vrock::ui::Application> app ) : BaseWidget( app )
    {
    }

    std::shared_ptr<vrock::pdf::Page> last_rendered_page = nullptr;
    std::shared_ptr<vrock::ui::Image> rendered_page = nullptr;

    void render( ) final
    {
        ImGui::Begin( "View", &visibility, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );
        if ( globals::documents.empty( ) )
        {
            ImGui::End( );
            return;
        }

        ImGui::BeginTabBar( "view_files" );

        std::size_t i = 0;
        for ( auto &doc : globals::documents )
        {
            ImGui::PushID( i );
            if ( ImGui::TabItemButton( doc->get_file_name( ).c_str( ),
                                       ImGuiTabItemFlags_SetSelected * ( i == globals::selected_document ) ) )
            {
                vrock::utils::ScopedTimer timer( []( auto t ) { std::cout << t << "ms" << std::endl; } );
                //                globals::selected_image.clear( );
                //                globals::selected_document = i;
                //                for ( auto page : doc->get_pages( ) )
                //                    for ( auto [ k, img ] : page->resources->images )
                //                        globals::selected_image.push_back( std::make_shared<vrock::ui::Image>(
                //                            img->get_width( ), img->get_height( ), vrock::ui::ImageFormat::RGBA,
                //                            img->as_rgba( )->data( ) ) );
            }

            ImGui::PopID( );
            i++;
        }

        //        for ( auto img : globals::selected_image )
        //            ImGui::Image( img->get_descriptor_set( ),
        //                          { (float)img->get_width( ) * 1.5f, (float)img->get_height( ) * 1.5f } );

        for ( auto img : globals::rendered_pages )
            ImGui::Image( img->get_descriptor_set( ),
                          { (float)img->get_width( ) * globals::scale, (float)img->get_height( ) * globals::scale } );

        ImGui::EndTabBar( );

        ImGui::End( );
    }
};