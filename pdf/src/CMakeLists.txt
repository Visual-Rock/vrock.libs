add_library(vrockpdf)

find_package(ZLIB REQUIRED)

target_include_directories(vrockpdf PUBLIC ./include/)
target_sources(vrockpdf
        PUBLIC
        include/vrock/pdf/structure/PDFObjects.hpp

        PRIVATE
        src/PDFDocument.cpp

        src/parser/BaseParser.cpp
        src/parser/ContentStreamParser.cpp
        src/parser/PDFObjectParser.cpp

        src/structure/Colorspaces.cpp
        src/structure/Font.cpp
        src/structure/PDFContext.cpp
        src/structure/PDFDataStructures.cpp
        src/structure/PDFEncryption.cpp
        src/structure/PDFFilters.cpp
        src/structure/PDFImage.cpp
        src/structure/PDFObjects.cpp
        src/structure/PDFPageTree.cpp
        src/structure/PDFStreams.cpp
        src/structure/Rectangle.cpp
        src/structure/RenderableObject.cpp
)
target_include_directories(vrockpdf PRIVATE ../../external/zlib/)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(vrockpdf PRIVATE "-Wno-tautological-undefined-compare")
    target_compile_options(vrockpdf PRIVATE "-Wno-unknown-attributes")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(vrockpdf PRIVATE "-Wno-tautological-undefined-compare")
    target_compile_options(vrockpdf PRIVATE "-Wno-unknown-attributes")
endif ()

target_link_libraries(vrockpdf PUBLIC vrockutils)
target_link_libraries(vrockpdf PRIVATE vrocksecurity icu ZLIB::ZLIB stb_image)