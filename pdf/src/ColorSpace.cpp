module;

import vrock.utils.ByteArray;
import vrock.pdf.PDFBaseObjects;

#include <cstdint>
#include <format>
#include <iostream>
#include <memory>

module vrock.pdf.ColorSpace;

namespace vrock::pdf
{
    IndexedColorSpace::IndexedColorSpace( std::shared_ptr<PDFArray> arr ) : ColorSpace( ColorSpaceType::Indexed )
    {
        if ( auto highval = arr->get<PDFInteger>( 2 ) )
            highest_value = highval->value;
        if ( auto str = arr->get<PDFString>( 3 ) )
            map = str->get_byte_array( );
        else if ( auto stream = arr->get<PDFStream>( 3 ) )
            map = stream->data;
    }

    auto IndexedColorSpace::convert_to_rgb( std::shared_ptr<utils::ByteArray<>> data,
                                            std::shared_ptr<PDFBaseObject> params )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        std::int32_t width = 0, height = 0, bpp = 8;
        if ( auto dict = params->to<PDFDictionary>( ) )
        {
            if ( auto w = dict->get<PDFInteger>( "Width" ) )
                width = w->value;
            if ( auto h = dict->get<PDFInteger>( "Height" ) )
                height = h->value;
            if ( auto b = dict->get<PDFInteger>( "BitsPerComponent" ) )
                bpp = b->value;
        }

        for ( int i = 0; i < map->size( ) / 3; ++i )
        {
            std::cout << std::format( "{}: {} {} {}", i, map->at( i * 3 ), map->at( i * 3 + 1 ), map->at( i * 3 + 2 ) )
                      << std::endl;
        }

        auto converted = std::make_shared<utils::ByteArray<>>( width * height * ( bpp / 8 ) * 3 );
        try
        {
            for ( auto i = 0; i < ( width * height ) - 1; ++i )
            {
                if ( data->at( i ) > highest_value )
                    continue;
                converted->at( i * 3 ) = map->at( data->at( i ) );
                converted->at( i * 3 + 1 ) = map->at( data->at( i ) + 1 );
                converted->at( i * 3 + 2 ) = map->at( data->at( i ) + 2 );
            }
        }
        catch ( std::exception &e )
        {
            std::cout << "tmp" << std::endl;
        }

        return converted;
    }

    auto to_colorspace( std::shared_ptr<PDFBaseObject> obj ) -> std::shared_ptr<ColorSpace>
    {
        if ( obj == nullptr )
            return nullptr;

        if ( auto color_space = obj->to<PDFArray>( ) )
        {
            if ( auto type = color_space->get<PDFName>( 0 ) )
            {
                if ( type->name == "Indexed" )
                    return std::make_shared<IndexedColorSpace>( color_space );
            }
        }
        return nullptr;
    }
} // namespace vrock::pdf