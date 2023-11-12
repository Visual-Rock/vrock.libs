find_package(unofficial-skia CONFIG REQUIRED)

add_library(vrockui)
target_include_directories(vrockui PUBLIC ./include/)
target_sources(vrockui PRIVATE
        src/Color.cpp

        src/gui/native/NativeVisual.cpp
        src/gui/native/NativeWindow.cpp
        src/gui/native/RasterVisual.cpp

        #src/gui/Panel.cpp
        #src/gui/TextBlock.cpp
        src/gui/Window.cpp

        src/RenderBackends/OpenGLBackend.cpp
)

if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)

    include(FindPkgConfig)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(XCB_IMAGE REQUIRED xcb-image)

    target_sources(vrockui PRIVATE
            src/gui/native/linux/XcbVisual.cpp
            src/gui/native/linux/XcbWindow.cpp
            src/gui/native/linux/XlibWindow.cpp
    )

    target_link_libraries(vrockui PRIVATE X11::X11 X11::xcb ${XCB_IMAGE_LIBRARIES} X11::X11_xcb)
    target_include_directories(vrockui PRIVATE ${XCB_IMAGE_INCLUDE_DIRS})
endif ()

target_link_libraries(vrockui PRIVATE unofficial::skia::skia)