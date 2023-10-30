add_library(stb_image)
target_sources(stb_image PRIVATE stb/stb.cpp)
target_include_directories(stb_image PUBLIC stb)


if (${LOG})
    add_subdirectory(spdlog)
endif ()

if (${VROCKLIBS_SECURITY})
    option(CRYPTOPP_BUILD_TESTING "" OFF)
    option(CRYPTOPP_INSTALL "" OFF)
    add_subdirectory(cryptopp)
endif ()

if (${UI})
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
    set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
    add_subdirectory(glfw)
    if (MSVC)
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
    endif ()

    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "")
    set(ONLY_LIBS ON CACHE BOOL "")
    add_subdirectory(glew)
    add_library(imgui STATIC)

    set(SKIP_INSTALL_ALL ON CACHE BOOL "")
    add_subdirectory(freetype)

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
    target_link_libraries(imgui PRIVATE freetype)
endif ()

if (${PDF})
    add_subdirectory(zlib)
    add_subdirectory(icu)
endif ()