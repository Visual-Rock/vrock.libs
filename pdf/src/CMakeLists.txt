find_package(ICU COMPONENTS uc data REQUIRED)
find_package(ZLIB REQUIRED)

add_library(vrockpdf)
target_sources(vrockpdf
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        PDFDocument.ixx
        PDFPageObjects.ixx
        PDFEncryption.ixx
        PDFDataStructures.ixx

        parser/BaseParser.ixx
        parser/PDFObjectParser.ixx
        parser/PDFObjects.ixx
        parser/ContentStreamParser.ixx
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
)

target_link_libraries(vrockpdf PUBLIC vrockutils vrocksecurity vrocklog ICU::uc ICU::data zlibstatic stb_image)