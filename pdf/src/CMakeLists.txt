add_library(vrockpdf)
target_sources(vrockpdf
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        PDFDocument.ixx
        PDFPageObjects.ixx
        PDFEncryption.ixx
        PDFDataStructures.ixx

        Math.ixx

        parser/BaseParser.ixx
        parser/PDFObjectParser.ixx
        parser/PDFObjects.ixx
        parser/ContentStreamParser.ixx

        structure/PDFPageTree.ixx
        structure/RenderableObject.ixx
        PRIVATE
        PDFDocument.cpp
        PDFEncryption.cpp
        parser/ColorSpace.cpp

        parser/PDFPageObjects.cpp
        parser/BaseParser.cpp
        parser/PDFObjectParser.cpp
        parser/PDFObjects.cpp
        parser/PDFFilter.cpp
        parser/ContentStreamParser.cpp

        structure/PDFPageTree.cpp
        structure/RenderableObject.cpp
)
target_include_directories(vrockpdf PRIVATE ../../external/zlib/)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(vrockpdf PRIVATE "-Wno-tautological-undefined-compare")
endif ()

target_link_libraries(vrockpdf PRIVATE vrockutils vrocksecurity vrocklog icu zlibstatic stb_image)