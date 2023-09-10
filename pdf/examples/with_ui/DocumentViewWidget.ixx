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

        // render selected page
        auto selected = globals::selected_page;
        if ( last_rendered_page != globals::selected_page )
        {
            auto page = globals::selected_page;
            if ( page )
            {
                auto page = globals::selected_page;
                auto width = (std::int32_t)page->media_box->upper_right.x.units;
                auto height = (std::int32_t)page->media_box->upper_right.y.units;
                auto data = std::make_shared<vrock::utils::ByteArray<>>( width * height * 4 );
                std::memset( data->data( ), 0xff, data->size( ) );

                // Render logic

                // render image
                page->get_images( ).for_each(
                    [ & ]( auto image ) { render_image_to_bytearray( data, image, width, height ); } );

                rendered_page =
                    std::make_shared<vrock::ui::Image>( width, height, vrock::ui::ImageFormat::RGBA, data->data( ) );
            }
            last_rendered_page = globals::selected_page;
        }

        if ( rendered_page )
            ImGui::Image( rendered_page->get_descriptor_set( ),
                          { (float)rendered_page->get_width( ) * globals::scale,
                            (float)rendered_page->get_height( ) * globals::scale } );

        ImGui::EndTabBar( );

        ImGui::End( );
    }
};

auto render_image_to_bytearray( std::shared_ptr<vrock::utils::ByteArray<>> array,
                                std::shared_ptr<vrock::pdf::Image> image, std::int32_t page_width,
                                std::int32_t page_height ) -> void
{
    auto initial_width = (std::int32_t)image->image->get_width( );
    auto initial_height = (std::int32_t)image->image->get_height( );
    auto render_width = (std::int32_t)image->scale.x.units;
    auto render_height = (std::int32_t)image->scale.y.units;
    auto resized_image = std::make_shared<vrock::utils::ByteArray<>>( render_width * render_height * 4 );
    auto image_data = image->image->as_rgba( );

    // rescale image
    stbir_resize_uint8( image_data->data( ), initial_width, initial_height, 0, resized_image->data( ), render_width,
                        render_height, 0, 4 );

    // calculate position
    auto position_x = (std::int32_t)image->position.x.units;
    // to get the position relative to upper left corner
    auto position_y = page_height - (std::int32_t)image->position.y.units;
    position_y -= render_height; // to get position of upper corner not lower

    std::size_t start_offset = 4 * ( page_width * position_y + position_x );
    for ( std::size_t i = 0; i < render_height; ++i )
    {
        auto offset = start_offset + ( page_width * i * 4 );
        std::memcpy( array->data( ) + offset, resized_image->data( ) + ( ( render_width * i ) * 4 ), render_width * 4 );
    }
}