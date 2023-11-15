find_package(unofficial-skia CONFIG REQUIRED)
find_package(glfw3 REQUIRED)

add_library(vrockui
        src/Application.cpp)
target_include_directories(vrockui PUBLIC ./include/)
target_sources(vrockui PRIVATE
        src/Color.cpp

        #src/gui/Panel.cpp
        #src/gui/TextBlock.cpp
        src/gui/Window.cpp
)

message("emscript: " $ENV{EMSDK})

if ()
endif ()

target_link_libraries(vrockui PUBLIC unofficial::skia::skia glfw)