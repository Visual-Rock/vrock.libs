add_library(stb_image)
target_sources(stb_image PRIVATE stb/stb.cpp)
target_include_directories(stb_image PUBLIC stb)

if (${VROCKLIBS_UI})
    add_subdirectory(glfw)
endif ()

if (${PDF})
    add_subdirectory(zlib)
    add_subdirectory(icu)
endif ()