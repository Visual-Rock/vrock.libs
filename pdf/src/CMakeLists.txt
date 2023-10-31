add_library(vrockpdf)
target_include_directories(vrockpdf PUBLIC ./include/)
target_sources(vrockpdf
        PUBLIC
        include/vrock/pdf/structure/PDFObjects.hpp

        PRIVATE
        src/PDFDocument.cpp

        src/parser/PDFObjectParser.cpp

        src/structure/PDFObjects.cpp
        src/structure/PDFStreams.cpp
        src/structure/PDFFilters.cpp
        src/structure/PDFContext.cpp
        src/structure/PDFEncryption.cpp
        src/structure/PDFPageTree.cpp
)
target_include_directories(vrockpdf PRIVATE ../../external/zlib/)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(vrockpdf PRIVATE "-Wno-tautological-undefined-compare")
    target_compile_options(vrockpdf PRIVATE "-Wno-unknown-attributes")
endif ()

target_link_libraries(vrockpdf PUBLIC vrockutils)
target_link_libraries(vrockpdf PRIVATE vrocksecurity icu zlibstatic stb_image)