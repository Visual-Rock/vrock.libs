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

if (${VROCKLIBS_UI})
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
    set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
    add_subdirectory(glfw)
    if (MSVC)
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
    endif ()

    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "")
    set(ONLY_LIBS ON CACHE BOOL "")
    add_subdirectory(glew)
endif ()

if (${PDF})
    add_subdirectory(zlib)
    add_subdirectory(icu)
endif ()