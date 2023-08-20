add_library(stb_image)
target_sources(stb_image PRIVATE stb/stb.cpp)
target_include_directories(stb_image PUBLIC stb)


if (${LOG})
    add_subdirectory(spdlog)
endif ()

if (${UI})
    add_subdirectory(fmt)

    add_library(imgui STATIC)

    pkg_check_modules(FREETYPE IMPORTED_TARGET freetype2)

    target_sources(imgui
            PRIVATE
            imgui/imgui.cpp
            imgui/imgui_demo.cpp
            imgui/imgui_draw.cpp
            imgui/imgui_tables.cpp
            imgui/imgui_widgets.cpp
            imgui/backends/imgui_impl_glfw.cpp
            imgui/backends/imgui_impl_vulkan.cpp
            imgui/misc/freetype/imgui_freetype.cpp
            imgui/misc/cpp/imgui_stdlib.cpp
    )
    target_include_directories(imgui PUBLIC imgui imgui/backends)
    target_link_libraries(imgui PRIVATE PkgConfig::FREETYPE)
endif ()