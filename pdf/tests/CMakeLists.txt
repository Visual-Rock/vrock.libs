enable_testing()

file(COPY ../pdfs DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
add_executable(pdf_tests
        PDFDecryption.test.cpp

        parser/BaseParser.test.cpp
        parser/ContentStreamParser.test.cpp
        parser/PDFObjectParser.test.cpp

        structure/PDFFilter.test.cpp
        #structure/Functions.test.cpp
        #structure/Image.test.cpp
)

target_link_libraries(
        pdf_tests PRIVATE
        vrockpdf
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(pdf_tests)