find_package(Vulkan REQUIRED)

add_library(vrockui)
target_sources(vrockui
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        FileDialog.ixx
        Widgets.ixx
        Application.ixx
        Image.ixx
        KeyCode.ixx
        PRIVATE
        tinyfiledialog/tinyfiledialogs.c
        ../../external/stb/stb.cpp
        imgui_theme/spectrum.cpp

        FileDialog.cpp
        Widgets.cpp
        Application.cpp
        Image.cpp
)

target_link_libraries(vrockui PUBLIC vrockutils vrocklog imgui Vulkan::Vulkan stb_image)
target_link_libraries(vrockui PRIVATE glfw libglew_static)