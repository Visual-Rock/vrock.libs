find_package(unofficial-skia CONFIG REQUIRED)
find_package(X11)

add_library(vrockui)
target_include_directories(vrockui PUBLIC ./include/)
target_sources(vrockui PRIVATE
        src/Color.cpp

        src/gui/native/RasterVisual.cpp

        #src/gui/Panel.cpp
        #src/gui/TextBlock.cpp

        src/RenderBackends/OpenGLBackend.cpp
)

if (UNIX AND NOT APPLE)
    target_sources(vrockui PRIVATE
            src/gui/native/linux/XcbWindow.cpp
            src/gui/native/linux/XlibWindow.cpp
    )
    target_link_libraries(vrockui PRIVATE X11:X11)
endif ()

target_link_libraries(vrockui PRIVATE unofficial::skia::skia)