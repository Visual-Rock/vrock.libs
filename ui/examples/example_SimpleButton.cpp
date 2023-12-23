#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkDeferredDisplayListRecorder.h"
#include "include/core/SkFont.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkSurfaceCharacterization.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLAssembleInterface.h"
#include "include/gpu/gl/GrGLInterface.h"

#include <vrock/utils/Timer.hpp>

void framebuffer_size_callback( GLFWwindow *window, int width, int height );

void processInput( GLFWwindow *window );

void error_callback( int error, const char *description )
{
    fputs( description, stderr );
}

GrDirectContext *grDirectContext = nullptr;
SkSurface *skSurface = nullptr;

auto fn( void *, const char *p ) -> GrGLFuncPtr
{
    return glfwGetProcAddress( p );
}

void init_Skia( int width, int height )
{
    // auto interface = GrGLMakeAssembledInterface( nullptr, &fn );

    grDirectContext = GrDirectContext::MakeGL( ).release( );
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType; // kRGBA_8888_SkColorType;
    GrBackendRenderTarget backendRenderTarget( width, height, 0, 0, framebufferInfo );
    skSurface = SkSurface::MakeFromBackendRenderTarget( grDirectContext, backendRenderTarget,
                                                        kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr )
                    .release( );
    if ( skSurface == nullptr )
        abort( );
}

void clearnup_skia( )
{
    delete skSurface;
    delete grDirectContext;
}

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main( )
{
    GLFWwindow *window;
    glfwSetErrorCallback( error_callback );
    if ( !glfwInit( ) )
    {
        exit( EXIT_FAILURE );
    }

    window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );
    if ( !window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        exit( EXIT_FAILURE );
    }
    glfwMakeContextCurrent( window );

    init_Skia( WIDTH, HEIGHT );

    glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
    if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    SkCanvas *canvas = skSurface->getCanvas( );

    while ( !glfwWindowShouldClose( window ) )
    {
        processInput( window );
        {
            SkPaint paint;
            paint.setColor( SK_ColorWHITE );
            canvas->drawPaint( paint );

            vrock::utils::ScopedTimer<> t( []( auto x ) { std::cout << x << std::endl; } );

            SkPaint p;
            p.setAntiAlias( true );
            p.setStyle( SkPaint::kStroke_Style );
            p.setStrokeWidth( 10 );

            for ( int r = 0; r <= 15000; r++ )
            {
                p.setColor( SkColorSetRGB( 255, 0, 255 ) );
                SkVector corners[] = { { 15, 17 }, { 17, 19 }, { 19, 15 }, { 15, 15 } };
                SkRRect rect = SkRRect::MakeRect( SkRect::MakeWH( 100, 50 ) );
                rect.setRectRadii( SkRect::MakeWH( 100, 50 ), corners );
                SkPaint paint;
                auto col = SkColors::kCyan;
                paint.setColor( col );
                canvas->drawRRect( rect, paint );
                // canvas->drawRect( SkRect::MakeXYWH( r, g, b, 1 ), p );
            }
        }

        canvas->flush( );
        grDirectContext->flushAndSubmit( );

        glfwSwapBuffers( window );
        glfwPollEvents( );
    }

    clearnup_skia( );
    glfwDestroyWindow( window );
    glfwTerminate( );
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
void processInput( GLFWwindow *window )
{
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
        glfwSetWindowShouldClose( window, true );
}

void framebuffer_size_callback( GLFWwindow *window, int width, int height )
{
    glViewport( 0, 0, width, height );
}
