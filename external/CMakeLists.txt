add_library(stb_image)
target_sources(stb_image PRIVATE stb/stb.cpp)
target_include_directories(stb_image PUBLIC stb)

if (${VROCKLIBS_SECURITY})
    option(CRYPTOPP_BUILD_TESTING "" OFF)
    option(CRYPTOPP_INSTALL "" OFF)
    add_subdirectory(cryptopp)
endif ()

if (${VROCKLIBS_PDF})
    add_subdirectory(zlib)
    add_subdirectory(icu)
endif ()