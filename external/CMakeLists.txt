add_library(stb_image)
target_sources(stb_image PRIVATE stb/stb.cpp)
target_include_directories(stb_image PUBLIC stb)

if (${VROCKLIBS_PDF})
    if (NOT WIN32)
        add_subdirectory(icu)
    endif()
endif ()