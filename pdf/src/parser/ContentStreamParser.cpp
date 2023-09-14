module;

import vrock.pdf.PDFObjectParser;
import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFDataStructures;
import vrock.pdf.Math;
import vrock.pdf.RenderableObject;

#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

module vrock.pdf.ContentStreamParser;

namespace vrock::pdf
{
    ContentStreamParser::ContentStreamParser( std::string data, std::shared_ptr<ResourceDictionary> res_dict,
                                              std::shared_ptr<PDFContext> ctx )
        : PDFObjectParser( std::move( data ) ), res( std::move( res_dict ) )
    {
        std::cout << _string << std::endl;
        set_context( std::move( ctx ) );

        graphic_state_stack.emplace( );
        auto operator_seq = to_operator_array( );

        try
        {
            std::size_t i = 0;
            for ( auto op : operator_seq )
            {
                std::cout << i++ << ' ' << op->_operator << std::endl;
                if ( operator_fn.find( op->o ) != operator_fn.end( ) )
                {
                    operator_fn[ op->o ]( this, op );
                }
            }
            // else // if ( op->o == objects::ContentStreamOperator::Tr )
            //     for ( auto o : op->paramteres )
            //         log::get_logger( "pdf" )->log->debug( "{}: {}", op->_operator, o->to_string( ) );
        }
        catch ( const std::exception &e )
        {
            std::cout << e.what( ) << std::endl;
            // log::get_logger( "pdf" )->log->debug( "ContentStreamParser Exception: {}", e.what( ) );
        }
    }

    auto ContentStreamParser::parse_object( std::shared_ptr<PDFRef> ref, bool decrypt )
        -> std::shared_ptr<PDFBaseObject>
    {
        skip_whitespace( );
        if ( is_keyword( "true" ) )
            return std::make_shared<PDFBool>( true );
        if ( is_keyword( "false" ) )
            return std::make_shared<PDFBool>( false );
        if ( is_keyword( "null" ) )
            return std::make_shared<PDFNull>( );

        if ( _string[ _offset ] == '<' && _string[ _offset + 1 ] == '<' )
            return parse_dictionary( ref, decrypt );
        if ( _string[ _offset ] == '/' )
            return parse_name( );
        if ( _string[ _offset ] == '<' )
            return parse_hex_string( ref, decrypt );
        if ( _string[ _offset ] == '(' )
            return parse_string( ref, decrypt );
        if ( _string[ _offset ] == '[' )
            return parse_array( ref, decrypt );
        if ( is_digit_plus_minus_dot( _string[ _offset ] ) )
            return parse_number_or_reference( );
        if ( _string[ _offset ] >= 'A' && _string[ _offset ] <= 'z' )
            return parse_operator( );
        throw PDFParserException( std::format( "Unknown object: {}", _string.substr( _offset - 20, 50 ) ) );
    }

    auto ContentStreamParser::parse_operator( ) -> std::shared_ptr<PDFOperator>
    {
        if ( _string[ _offset ] == '\'' || _string[ _offset ] == '"' )
            return std::make_shared<PDFOperator>( _string.substr( _offset++, 1 ) );
        char i = 0;
        while ( _string.length( ) > _offset &&
                ( ( _string[ _offset + i ] >= 'A' && _string[ _offset + i ] <= 'Z' ) ||
                  ( _string[ _offset + i ] >= 'a' && _string[ _offset + i ] <= 'z' ) || _string[ _offset + i ] == '*' ||
                  _string[ _offset + i ] == '0' || _string[ _offset + i ] == '1' ) )
            i++;
        auto str = _string.substr( _offset, i );
        _offset += i;
        return std::make_shared<PDFOperator>( str );
    }

    auto ContentStreamParser::to_operator_array( ) -> std::vector<std::shared_ptr<PDFOperator>>
    {
        std::vector<std::shared_ptr<PDFOperator>> operators;
        std::vector<std::shared_ptr<PDFBaseObject>> params = { };
        while ( _string.length( ) > _offset )
        {
            auto obj = parse_object( );
            if ( obj->type == PDFObjectType::Operator )
            {
                auto op = obj->to<PDFOperator>( );
                op->paramteres = std::move( params );
                params = { };
                operators.emplace_back( op );
            }
            else
                params.push_back( obj );
            skip_whitespace( );
        }
        return operators;
    }

    auto operator_cm( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op ) -> void
    {
        std::array<double, 6> data = { };
        // check validity
        {
            if ( op->paramteres.size( ) != 6 )
                throw std::runtime_error( "gs operator parameter wrong" );
            for ( size_t i = 0; i < 6; i++ )
                data[ i ] = get_number( op->paramteres[ i ] );
        }
        // TODO: check if this is correct behavior
        mat3 mat{ { { data[ 0 ], data[ 1 ], 0 }, { data[ 2 ], data[ 3 ], 0 }, { data[ 4 ], data[ 5 ], 1 } } };
        parser->graphic_state_stack.top( ).current_transformation_matrix =
            mul( mat, parser->graphic_state_stack.top( ).current_transformation_matrix );
    }

    void operator_Do( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        auto &ctm = parser->graphic_state_stack.top( ).current_transformation_matrix;
        auto a = ctm[ 0 ][ 0 ];
        auto b = ctm[ 1 ][ 0 ];
        auto c = ctm[ 2 ][ 0 ];
        auto d = ctm[ 0 ][ 1 ];
        auto e = ctm[ 1 ][ 1 ];
        auto f = ctm[ 2 ][ 1 ];

        auto p = std::sqrt( a * a + b * b );
        auto r = ( a * e - b * d ) / ( p );
        auto q = ( a * d + b * e ) / ( a * e - b * d );

        std::shared_ptr<PDFName> name = nullptr;
        if ( op->paramteres.size( ) != 1 )
            goto _throw;
        name = op->paramteres[ 0 ]->to<PDFName>( );
        if ( !name )
            goto _throw;
        // check if image and if so add it to images
        if ( parser->res->images.contains( name->name ) )
        {
            // TODO: check if it's really working (with shear and rotation)
            // TODO: move to function
            auto img = std::make_shared<Image>( );
            img->position = Point{ Unit( c ), Unit( f ) };
            img->scale = Point{ Unit( p ), Unit( r ) };
            img->shear = q;
            img->rotation = std::atan2( b, a );
            img->image = parser->res->images[ name->name ];
            parser->images.push_back( img );
        }
        return;
    _throw:
        throw std::runtime_error( "Do operator parameter wrong" );
    }

    auto operator_gs( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op ) -> void
    {
        std::shared_ptr<PDFName> name = nullptr;
        if ( op->paramteres.size( ) != 1 )
            goto _throw;
        name = op->paramteres[ 0 ]->to<PDFName>( );
        if ( name == nullptr )
            goto _throw;

        if ( !parser->res->ext_g_state.contains( name->name ) )
            throw std::runtime_error( std::format( "gs {} not found!", name->name ) );
        parser->graphic_state_stack.top( ).apply_dict( parser->res->ext_g_state[ name->name ] );
        return;
    _throw:
        throw std::runtime_error( "gs operator parameter wrong" );
    }

    void operator_q( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        parser->graphic_state_stack.emplace( parser->graphic_state_stack.top( ) );
    }

    void operator_Q( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        parser->graphic_state_stack.pop( );
    }

    void operator_Tf( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 2 )
            throw std::runtime_error( "Tf operator parameters wrong" );
        std::string name;
        if ( auto n = op->paramteres[ 0 ]->to<PDFName>( ) )
            name = n->name;

        if ( !parser->res->fonts.contains( name ) )
            throw std::runtime_error( std::format( "font {} not found!", name ) );
        parser->graphic_state_stack.top( ).text_state.font = parser->res->fonts[ name ];
        parser->graphic_state_stack.top( ).text_state.font_size = get_number( op->paramteres[ 1 ] );
    }

    void operator_Tc( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 )
            throw std::runtime_error( "Tc operator parameter wrong" );
        parser->graphic_state_stack.top( ).text_state.character_spacing = get_number( op->paramteres[ 0 ] );
    }

    void operator_Tj( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 && op->paramteres[ 0 ]->type != PDFObjectType::String )
            throw std::runtime_error( "Tj operator parameter wrong" );
        // log::get_logger( "pdf" )->log->debug( "Text: {}", objects::get_string( op->paramteres[ 0 ] ) );
        auto text = std::make_shared<Text>( );
        parser->text.push_back( text );
        text->text = op->paramteres[ 0 ]->to<PDFString>( )->get_string( );
        text->offsets = { { text->text.length( ), 0 } };
        text->font_size = parser->graphic_state_stack.top( ).text_state.font_size;
        text->font = parser->graphic_state_stack.top( ).text_state.font;
    }

    void operator_TJ( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 )
            throw std::runtime_error( "TJ operator parameter wrong" );
        else if ( !op->paramteres[ 0 ]->is( PDFObjectType::Array ) )
            throw std::runtime_error( "TJ operator parameter wrong" );

        auto arr = op->paramteres[ 0 ]->to<PDFArray>( );
        auto text = std::make_shared<Text>( );
        parser->text.push_back( text );
        int32_t offset = 0;
        for ( auto i : arr->value )
        {
            if ( auto str = i->to<PDFString>( ) )
            {
                auto s = str->get_string( );
                text->text += s;
                text->offsets.emplace_back( s.length( ), offset );
            }
            else if ( i->is( PDFObjectType::Integer ) || i->is( PDFObjectType::Real ) )
                offset = (int32_t)get_number( i );
        }
        text->font_size = parser->graphic_state_stack.top( ).text_state.font_size;
        text->font = parser->graphic_state_stack.top( ).text_state.font;
    }

    void operator_TL( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 )
            throw std::runtime_error( "TL operator parameter wrong" );
        parser->graphic_state_stack.top( ).text_state.leading = (int32_t)get_number( op->paramteres[ 0 ] );
    }

    void operator_Tw( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 )
            throw std::runtime_error( "Tc operator parameter wrong" );
        parser->graphic_state_stack.top( ).text_state.word_spacing = get_number( op->paramteres[ 0 ] );
    }

    void operator_Tz( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 )
            throw std::runtime_error( "Tz operator parameter wrong" );
        parser->graphic_state_stack.top( ).text_state.horizontal_scaling = get_number( op->paramteres[ 0 ] );
    }

    void operator_w( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        if ( op->paramteres.size( ) != 1 )
            throw std::runtime_error( "w operator parameter wrong" );
        parser->graphic_state_stack.top( ).line_width = get_number( op->paramteres[ 0 ] );
    }
} // namespace vrock::pdf