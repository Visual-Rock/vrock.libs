pkg_check_modules(GLFW IMPORTED_TARGET glfw3)
pkg_check_modules(GLEW IMPORTED_TARGET glew)

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
        stb/stb.cpp
        imgui_theme/spectrum.cpp

        FileDialog.cpp
        Widgets.cpp
        Application.cpp
        Image.cpp
)

target_link_libraries(vrockui PUBLIC vrockutils vrocklog fmt imgui PkgConfig::GLFW PkgConfig::GLEW Vulkan::Vulkan)