#include "vrock/pdf/parser/ContentStreamParser.hpp"

namespace vrock::pdf
{
    auto mul( const mat3 &a, const mat3 &b ) -> mat3
    {
        mat3 res{ { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } };

        res[ 0 ][ 0 ] = a[ 0 ][ 0 ] * b[ 0 ][ 0 ] + a[ 0 ][ 1 ] * b[ 1 ][ 0 ] + a[ 0 ][ 2 ] * b[ 2 ][ 0 ];
        res[ 1 ][ 0 ] = a[ 1 ][ 0 ] * b[ 0 ][ 0 ] + a[ 1 ][ 1 ] * b[ 1 ][ 0 ] + a[ 1 ][ 2 ] * b[ 2 ][ 0 ];
        res[ 2 ][ 0 ] = a[ 2 ][ 0 ] * b[ 0 ][ 0 ] + a[ 2 ][ 1 ] * b[ 1 ][ 0 ] + a[ 2 ][ 2 ] * b[ 2 ][ 0 ];

        res[ 0 ][ 1 ] = a[ 0 ][ 0 ] * b[ 0 ][ 1 ] + a[ 0 ][ 1 ] * b[ 1 ][ 1 ] + a[ 0 ][ 2 ] * b[ 2 ][ 1 ];
        res[ 1 ][ 1 ] = a[ 1 ][ 0 ] * b[ 0 ][ 1 ] + a[ 1 ][ 1 ] * b[ 1 ][ 1 ] + a[ 1 ][ 2 ] * b[ 2 ][ 1 ];
        res[ 2 ][ 1 ] = a[ 2 ][ 0 ] * b[ 0 ][ 1 ] + a[ 2 ][ 1 ] * b[ 1 ][ 1 ] + a[ 2 ][ 2 ] * b[ 2 ][ 1 ];

        res[ 0 ][ 2 ] = a[ 0 ][ 0 ] * b[ 0 ][ 2 ] + a[ 0 ][ 1 ] * b[ 1 ][ 2 ] + a[ 0 ][ 2 ] * b[ 2 ][ 2 ];
        res[ 1 ][ 2 ] = a[ 1 ][ 0 ] * b[ 0 ][ 2 ] + a[ 1 ][ 1 ] * b[ 1 ][ 2 ] + a[ 1 ][ 2 ] * b[ 2 ][ 2 ];
        res[ 2 ][ 2 ] = a[ 2 ][ 0 ] * b[ 0 ][ 2 ] + a[ 2 ][ 1 ] * b[ 1 ][ 2 ] + a[ 2 ][ 2 ] * b[ 2 ][ 2 ];

        return res;
    }

    PDFOperator::PDFOperator( std::string op )
        : PDFBaseObject( PDFObjectType::Operator ), _operator( op ), o( string_to_operator[ op ] )
    {
    }

    template <>
    auto to_object_type<PDFOperator>( ) -> PDFObjectType
    {
        return PDFObjectType::Operator;
    }

    ContentStreamParser::ContentStreamParser( std::string data, std::shared_ptr<ResourceDictionary> res_dict,
                                              std::shared_ptr<PDFContext> ctx )
        : PDFObjectParser( std::move( data ) ), res( std::move( res_dict ) )
    {
        set_context( std::move( ctx ) );

        graphic_state_stack.push( GraphicState( ) );
        auto operator_seq = to_operator_array( );

        try
        {
            std::size_t i = 0;
            for ( auto op : operator_seq )
            {
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

    void check_operator_param_count( const std::shared_ptr<PDFOperator> &_operator, std::size_t count )
    {
        if ( _operator->paramteres.size( ) != count )
            throw std::runtime_error( std::format( "{} operator parameter wrong", _operator->_operator ) );
    }

    template <typename T, typename R>
    R check_type_and_get_value( const std::shared_ptr<PDFOperator> &_operator, std::size_t idx,
                                std::function<R( std::shared_ptr<T> )> fn, R _default = R( ) )
    {
        if ( auto obj = _operator->paramteres[ idx ]->to<T>( ) )
            return fn( obj );
        return _default;
    }

    double check_type_and_get_double( const std::shared_ptr<PDFOperator> &_operator, std::size_t idx )
    {
        return check_type_and_get_value<PDFNumber, double>(
            _operator, idx, []( auto n ) { return n->as_double( ); }, 0.0 );
    }

    std::int32_t check_type_and_get_int( const std::shared_ptr<PDFOperator> &_operator, std::size_t idx )
    {
        return check_type_and_get_value<PDFNumber, std::int32_t>(
            _operator, idx, []( auto n ) { return n->as_int( ); }, 0 );
    }

    std::string check_type_and_get_name( const std::shared_ptr<PDFOperator> &_operator, std::size_t idx )
    {
        return check_type_and_get_value<PDFName, std::string>(
            _operator, idx, []( auto n ) { return n->name; }, "" );
    }

    auto operator_cm( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op ) -> void
    {
        std::array<double, 6> data = { };
        check_operator_param_count( op, 6 );
        for ( size_t i = 0; i < 6; i++ )
            data[ i ] = check_type_and_get_double( op, i );

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

        check_operator_param_count( op, 1 );
        auto name = check_type_and_get_name( op, 0 );

        // check if image and if so add it to images
        if ( parser->res->images.contains( name ) )
        {
            // TODO: check if it's really working (with shear and rotation)
            // TODO: move to function
            auto img = std::make_shared<Image>( );
            img->position = Point{ Unit( c ), Unit( f ) };
            img->scale = Point{ Unit( p ), Unit( r ) };
            img->shear = q;
            img->rotation = std::atan2( b, a );
            img->image = parser->res->images[ name ];
            parser->images.push_back( img );
        }
    }

    auto operator_gs( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op ) -> void
    {
        check_operator_param_count( op, 1 );
        auto name = check_type_and_get_name( op, 0 );

        if ( !parser->res->ext_g_state.contains( name ) )
            throw std::runtime_error( std::format( "gs '{}' not found!", name ) );
        parser->graphic_state_stack.top( ).apply_dict( parser->res->ext_g_state[ name ] );
    }

    void operator_q( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        GraphicState tmp = parser->graphic_state_stack.top( );
        parser->graphic_state_stack.push( tmp );
    }

    void operator_Q( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        parser->graphic_state_stack.pop( );
    }

    void operator_Tf( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 2 );
        auto font = check_type_and_get_name( op, 0 );

        if ( !parser->res->fonts.contains( font ) )
            throw std::runtime_error( std::format( "font '{}' not found!", font ) );

        parser->graphic_state_stack.top( ).text_state.font = parser->res->fonts[ font ];
        parser->graphic_state_stack.top( ).text_state.font_size = check_type_and_get_int( op, 1 );
    }

    void operator_Tc( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );
        parser->graphic_state_stack.top( ).text_state.character_spacing = check_type_and_get_double( op, 0 );
    }

    void operator_Tj( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );

        auto text = std::make_shared<Text>( );
        parser->text.push_back( text );
        text->text = check_type_and_get_value<PDFString, std::string>(
            op, 0, []( auto s ) { return s->get_string( ); }, "" );
        text->offsets = { { text->text.length( ), 0 } };
        text->font_size = parser->graphic_state_stack.top( ).text_state.font_size;
        text->font = parser->graphic_state_stack.top( ).text_state.font;
    }

    void operator_TJ( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );
        auto arr = check_type_and_get_value<PDFArray, std::vector<std::shared_ptr<PDFBaseObject>>>(
            op, 0, []( auto a ) { return a->value; }, { } );

        auto text = std::make_shared<Text>( );
        parser->text.push_back( text );
        int32_t offset = 0;
        for ( const auto &i : arr )
        {
            if ( auto str = i->to<PDFString>( ) )
            {
                auto s = str->get_string( );
                text->text += s;
                text->offsets.emplace_back( s.length( ), offset );
            }
            else if ( auto num = i->to<PDFNumber>( ) )
                offset = num->as_int( );
        }
        text->font_size = parser->graphic_state_stack.top( ).text_state.font_size;
        text->font = parser->graphic_state_stack.top( ).text_state.font;
    }

    void operator_TL( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );
        parser->graphic_state_stack.top( ).text_state.leading = check_type_and_get_int( op, 0 );
    }

    void operator_Tw( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );
        parser->graphic_state_stack.top( ).text_state.word_spacing = check_type_and_get_double( op, 0 );
    }

    void operator_Tz( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );
        parser->graphic_state_stack.top( ).text_state.horizontal_scaling = check_type_and_get_double( op, 0 );
    }

    void operator_w( ContentStreamParser *parser, std::shared_ptr<PDFOperator> op )
    {
        check_operator_param_count( op, 1 );
        parser->graphic_state_stack.top( ).line_width = check_type_and_get_double( op, 0 );
    }
} // namespace vrock::pdf
