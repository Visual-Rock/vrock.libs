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
        structure/Image.ixx
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
)
target_include_directories(vrockpdf PRIVATE ../../external/zlib/)
target_link_libraries(vrockpdf PRIVATE vrockutils vrocksecurity vrocklog icu zlibstatic stb_image)