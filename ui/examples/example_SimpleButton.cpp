#include "include/core/SkColorSpace.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <vrock/ui.hpp>

SkCanvas *canvas;
SkImageInfo info;
int width, height;
GLFWwindow *window;
sk_sp<SkSurface> surface;
sk_sp<GrDirectContext> context;

SkPaint getPaint( SkColor s )
{
    SkPaint paint;
    paint.setColor( s );
    return paint;
}

GrBackendRenderTarget t;

void SetSurfaceAndCanvas( )
{
    //    sk_sp<const GrGLInterface> interface = nullptr;
    //    sk_sp<GrDirectContext> context = GrDirectContext::MakeGL( interface );

    GrContextOptions options;
    context = GrDirectContext::MakeGL( nullptr, options );

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;
    SkColorType colorType;
    colorType = kRGBA_8888_SkColorType;
    t = GrBackendRenderTarget( width, height, 0, 0, framebufferInfo );
    surface = SkSurface::MakeFromBackendRenderTarget( context.get( ), t, kTopLeft_GrSurfaceOrigin, colorType, nullptr,
                                                      nullptr );

    // info = SkImageInfo::Make( width, height, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType );
    // surface = SkSurface::MakeRaster( info );
    //    surface = SkSurface::MakeRenderTarget( context.get( ), SkBudgeted::kNo, info );
    canvas = surface->getCanvas( );
    canvas->translate( 0.0f, height );
    canvas->scale( 1.0f, -1.0f );
}

void Draw( )
{
    canvas->drawRect( SkRect::MakeXYWH( 0, 0, width, height ), getPaint( SK_ColorWHITE ) );
    canvas->drawRect( SkRect::MakeXYWH( 0, 0, 250, 250 ), getPaint( SK_ColorRED ) );

    if ( auto dContext = GrAsDirectContext( canvas->recordingContext( ) ) )
    {
        dContext->flushAndSubmit( );
    }
    std::vector<uint32_t> pixels( width * height * 4 );
    surface->readPixels( info, pixels.data( ), width * sizeof( uint32_t ), 0, 0 );
    glDrawPixels( width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data( ) );
    glfwSwapBuffers( window );
}

void frame_buffer_size_callback( GLFWwindow *fwindow, int w, int h )
{
    width = w;
    height = h;
    SetSurfaceAndCanvas( );
    Draw( );
}

int main( )
{
    glfwInit( );
    //    glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
    window = glfwCreateWindow( 500, 500, "Skia Test", nullptr, nullptr );
    glfwMakeContextCurrent( window );
    glfwGetFramebufferSize( window, &( width ), &( height ) );
    glfwSetFramebufferSizeCallback( window, frame_buffer_size_callback );

    SetSurfaceAndCanvas( );

    while ( !glfwWindowShouldClose( window ) )
    {
        Draw( );
        glfwPollEvents( );
    }
    glfwDestroyWindow( window );
    glfwTerminate( );
    return 0;
}
