module;

import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDataStructures;

#include <memory>
#include <string>

#include <imgui.h>

export module RenderHelper;

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