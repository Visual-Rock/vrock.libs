find_package(Cairo)

add_library(vrockui)
target_include_directories(vrockui PUBLIC ./include/)
target_sources(vrockui PRIVATE
        src/Color.cpp

        #src/gui/Panel.cpp
        #src/gui/TextBlock.cpp

        src/RenderBackends/X11RenderBackend.cpp
)
target_link_libraries(vrockui PRIVATE Cairo::Cairo)