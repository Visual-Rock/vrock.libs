#include <blend2d.h>
#include <iostream>

#include <vrock/pdf/PDFDocument.hpp>
#include <vrock/utils/Timer.hpp>

using namespace vrock::pdf;

auto check_res( BLResult res ) -> void
{
    if ( res != BL_SUCCESS )
        throw std::runtime_error( std::format( "Failed to load a font (err={})\n", res ) );
}

int main( )
{
    vrock::utils::ScopedTimer timer( []( auto t ) { std::cout << t << "ms" << std::endl; } );

    auto doc = PDFDocument( "simple.pdf" );

    for ( int i = 0; i < doc.get_page_count( ); i++ )
    {
        auto page = doc.get_page( i );
        BLImage img( page->media_box->get_width( ).units, page->media_box->get_height( ).units, BL_FORMAT_PRGB32 );
        BLContext ctx( img );

        ctx.clearAll( );

        ctx.setFillStyle( BLRgba32( 0xFFFFFFFF ) );
        ctx.fillRect( 0, 0, page->media_box->get_width( ).units, page->media_box->get_height( ).units );
        for ( const auto &text : page->get_text( ) )
        {
            for ( auto string : text->strings )
            {
                // only truetype fonts are supported and the font file must be present (14 Standard Fonts are not
                // supported)
                if ( string->font->font_type != FontType::TrueType || string->font->descriptor->font_file2 == nullptr )
                {
                    vrock::log::get_logger( "pdf" )->info( "font type not supported or not embeded" );
                    continue;
                }
                else
                    vrock::log::get_logger( "pdf" )->info( "text \"{}\" is being displayed", string->text );
                BLFontData data;
                auto res = data.createFromData( string->font->descriptor->font_file2->data.c_str( ),
                                                string->font->descriptor->font_file2->data.size( ) );
                check_res( res );

                BLFontFace face;
                res = face.createFromData( data, 0 );
                check_res( res );

                BLFont font;
                res = font.createFromFace( face, string->font_size );
                check_res( res );

                ctx.setFillStyle( BLRgba32( 0xff000000 ) );
                ctx.rotate( string->rotation );
                ctx.scale( string->scale.x.units, string->scale.y.units );
                auto str = string->text;
                res = ctx.fillUtf8Text( BLPoint( string->position.x.units,
                                                 page->media_box->get_height( ).units - string->position.y.units ),
                                        font, str.c_str( ), str.size( ) );
                check_res( res );
            }
        }
        ctx.end( );
        img.writeToFile( std::format( "render_{}.png", i ).c_str( ) );
    }
}