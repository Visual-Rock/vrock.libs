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
        ColorSpace.ixx

        parser/BaseParser.ixx
        parser/PDFObjectParser.ixx
        parser/PDFObjects.ixx
        parser/ContentStreamParser.ixx
        PRIVATE
        PDFDocument.cpp
        PDFEncryption.cpp
        ColorSpace.cpp

        parser/PDFPageObjects.cpp
        parser/BaseParser.cpp
        parser/PDFObjectParser.cpp
        parser/PDFObjects.cpp
        parser/PDFFilter.cpp
        parser/ContentStreamParser.cpp
)

target_link_libraries(vrockpdf PUBLIC vrockutils vrocksecurity vrocklog ICU::uc ICU::data ZLIB::ZLIB stb_image)