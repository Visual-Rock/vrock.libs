module;

import vrock.utils.ByteArray;

import vrock.ui.Image;

import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDataStructures;
import vrock.pdf.PDFPageTree;
import vrock.pdf.RenderableObject;

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#include <imgui.h>

#include <stb_image_resize.h>

export module RenderHelper;

export auto render_image_to_bytearray( std::shared_ptr<vrock::utils::ByteArray<>> array,
                                       std::shared_ptr<vrock::pdf::Image> image, std::int32_t page_width,
                                       std::int32_t page_height ) -> void
{
    auto initial_width = (std::int32_t)image->image->get_width( );
    auto initial_height = (std::int32_t)image->image->get_height( );
    auto render_width = std::abs( (std::int32_t)image->scale.x.units );
    auto render_height = std::abs( (std::int32_t)image->scale.y.units );
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

export auto page_to_image( std::shared_ptr<vrock::pdf::Page> page ) -> std::shared_ptr<vrock::ui::Image>
{
    auto width = (std::int32_t)page->media_box->upper_right.x.units;
    auto height = (std::int32_t)page->media_box->upper_right.y.units;
    auto data = std::make_shared<vrock::utils::ByteArray<>>( width * height * 4 );
    std::memset( data->data( ), 0xff, data->size( ) );

    // Render logic

    // render image
    page->get_images( ).for_each( [ & ]( auto image ) { render_image_to_bytearray( data, image, width, height ); } );

    return std::make_shared<vrock::ui::Image>( width, height, vrock::ui::ImageFormat::RGBA, data->data( ) );
}

export auto render_point( vrock::pdf::Point &point )
{
    ImGui::SliderFloat( "x", &point.x.units, 0.0f, 5000.0f );
    ImGui::SliderFloat( "y", &point.y.units, 0.0f, 5000.0f );
}

export auto render_rectangle( const std::shared_ptr<vrock::pdf::Rectangle> &rect, const std::string &name )
{
    if ( ImGui::TreeNode( name.c_str( ) ) )
    {
        if ( ImGui::TreeNode( "Lower Left" ) )
        {
            render_point( rect->lower_left );
            ImGui::TreePop( );
        }
        if ( ImGui::TreeNode( "Upper Right" ) )
        {
            render_point( rect->upper_right );
            ImGui::TreePop( );
        }
        rect->upper_left = { rect->lower_left.x, rect->upper_right.y };
        rect->lower_right = { rect->upper_right.x, rect->lower_left.y };
        ImGui::TreePop( );
    }
}