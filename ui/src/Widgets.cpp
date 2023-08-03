module;

#include <memory>
#include <vector>

module vrock.ui.Widgets;

import vrock.ui.Application;

namespace vrock::ui
{
    // BaseWidget::

    BaseWidget::~BaseWidget( )
    {
    }

    auto BaseWidget::on_update( ) -> void
    {
        for ( auto &child : children )
            child->on_update( );
        update( );
    }

    auto BaseWidget::on_render( ) -> void
    {
        if ( !visibility )
            return;
        for ( auto &child : children )
            child->on_render( );
        render( );
    }

    auto BaseWidget::on_after_render( ) -> void
    {
        for ( auto &child : children )
            child->on_after_render( );
        after_render( );
    }

    auto BaseWidget::get_visibility( ) const -> bool
    {
        return visibility;
    }

    auto BaseWidget::set_visibility( bool new_visibility ) -> void
    {
        visibility = new_visibility;
    }

    template <class T, class... Args>
        requires std::is_base_of_v<BaseWidget, T>
    auto BaseWidget::create_child( Args &&...args ) -> std::shared_ptr<T>
    {
        auto child = std::make_shared<T>( args... );
        children.push_back( child );
        child->setup( );
        return child;
    }
} // namespace vrock::ui