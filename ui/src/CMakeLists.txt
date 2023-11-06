add_library(vrockui)
target_include_directories(vrockui PUBLIC ./include/)
target_sources(vrockui PRIVATE src/RenderBackends/OpenGLBackend.cpp)
target_link_libraries(vrockui PRIVATE glfw libglew_static)