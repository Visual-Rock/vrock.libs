add_library(vrockui)
target_include_directories(vrockui PUBLIC ./include/)
target_sources(vrockui PRIVATE
        src/Color.cpp

        src/gui/Panel.cpp

        src/RenderBackends/OpenGLBackend.cpp
)
target_link_libraries(vrockui PRIVATE glfw libglew_static)