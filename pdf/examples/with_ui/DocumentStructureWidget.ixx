module;

#include <imgui.h>

import vrock.ui.Application;
import vrock.utils.FutureHelper;
import vrock.pdf.PDFDocument;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.Image;
import vrock.ui.FileDialog;
import vrock.ui.Image;

import Globals;
import RenderHelper;
import vrock.pdf.PDFPageTree;

#include <format>
#include <future>
#include <iostream>
#include <memory>
#include <string>

export module DocumentStructureWidget;

export class DocumentStructure : public vrock::ui::BaseWidget
{
public:
    DocumentStructure( std::shared_ptr<vrock::ui::Application> app ) : vrock::ui::BaseWidget( app )
    {
    }
    ~DocumentStructure( ) = default;

    void render( ) final
    {
        ImGui::Begin( "Structure" );
        if ( globals::documents.empty( ) )
        {
            ImGui::End( );
            return;
        }

        auto &doc = globals::documents[ globals::selected_document ];

        ImGui::DragFloat( "scale", &globals::scale, 0.05f, 0.0f, 4.0f );
        ImGui::LabelText( "Filename", "%s", doc->get_file_name( ).c_str( ) );

        if ( ImGui::Button( "Save" ) )
            doc->save( );
        ImGui::SameLine( );
        if ( ImGui::Button( "Save As" ) )
        {
            auto save_as_location = vrock::ui::save_file( "Save PDF", "", globals::pdf_file_filter );
            doc->save( save_as_location );
        }

        if ( ImGui::TreeNode( "Pages" ) )
        {
            for ( int i = 0; i < doc->get_page_count( ); ++i )
            {
                ImGui::PushID( i );
                if ( ImGui::TreeNode( std::format( "Page {}", i ).c_str( ) ) )
                {
                    if ( ImGui::Button( "render" ) )
                        globals::selected_page = doc->get_page( i );
                    render_page( doc->get_page( i ) );
                    ImGui::TreePop( );
                }
                ImGui::PopID( );
            }
            ImGui::TreePop( );
        }

        ImGui::End( );
    }
    // void terminate( ) final;

protected:
    auto render_font( const std::shared_ptr<vrock::pdf::Font> &font, const std::string &name ) -> void
    {
        if ( ImGui::TreeNode( name.c_str( ) ) )
        {
            ImGui::Text( "%s", std::format( "Base Font: {}", font->base_font ).c_str( ) );
            ImGui::Text( "%s", std::format( "Font Name: {}", font->font_name ).c_str( ) );
            ImGui::Text( "%s",
                         std::format( "First Char: {} | Last Char: {}", font->first_char, font->last_char ).c_str( ) );
            if ( ImGui::TreeNode( "Widths" ) )
            {
                for ( auto w : font->widths )
                    ImGui::Text( "%s", std::to_string( w ).c_str( ) );
                ImGui::TreePop( );
            }
            ImGui::TreePop( );
        }
    }

    auto render_page( const std::shared_ptr<vrock::pdf::Page> &page ) -> void
    {
        render_rectangle( page->media_box, "Media Box" );
        render_rectangle( page->crop_box, "Crop Box" );
        render_rectangle( page->bleed_box, "Bleed Box" );
        render_rectangle( page->trim_box, "Trim Box" );
        render_rectangle( page->art_box, "Art Box" );
        ImGui::SliderInt( "Rotation", &page->rotation, 0, 270 );
        if ( ImGui::TreeNode( "Content Images" ) )
        {
            std::size_t i = 0;
            for ( const auto &image : page->get_images( ) )
            {
                if ( ImGui::TreeNode( std::format( "Image: {}", ++i ).c_str( ) ) )
                {
                    if ( ImGui::Button( "Save" ) )
                    {
                        auto save_as_location = vrock::ui::save_file( "Save Image", "", globals::image_file_filter );
                        image->image_data->save( save_as_location );
                    }
                    ImGui::SeparatorText( "render values" );
                    ImGui::Text( "position" );
                    render_point( image->position );
                    ImGui::Text( "size" );
                    render_point( image->scale );

                    ImGui::SeparatorText( "source values" );
                    ImGui::Text( "%s", std::format( "Width: {}", image->image_data->get_width( ) ).c_str( ) );
                    ImGui::Text( "%s", std::format( "Height: {}", image->image_data->get_height( ) ).c_str( ) );
                    ImGui::Text( "%s", std::format( "BPP: {}", image->image_data->get_bits_per_pixel( ) ).c_str( ) );
                    ImGui::TreePop( );
                }
            }
            ImGui::TreePop( );
        }
        if ( ImGui::TreeNode( "Resources" ) )
        {
            if ( ImGui::TreeNode( "Fonts" ) )
            {
                for ( auto [ name, font ] : page->resources->fonts )
                    render_font( font, name );
                ImGui::TreePop( );
            }
            if ( ImGui::TreeNode( "Images" ) )
            {
                for ( auto [ name, image ] : page->resources->images )
                {
                    if ( ImGui::TreeNode( name.c_str( ) ) )
                    {
                        if ( ImGui::Button( "Save" ) )
                        {
                            auto save_as_location =
                                vrock::ui::save_file( "Save Image", "", globals::image_file_filter );
                            image->save( save_as_location );
                        }
                        ImGui::Text( "%s", std::format( "Width: {}", image->get_width( ) ).c_str( ) );
                        ImGui::Text( "%s", std::format( "Height: {}", image->get_height( ) ).c_str( ) );
                        ImGui::Text( "%s", std::format( "BPP: {}", image->get_bits_per_pixel( ) ).c_str( ) );
                        ImGui::TreePop( );
                    }
                }
                ImGui::TreePop( );
            }
            ImGui::TreePop( );
        }
    }
};