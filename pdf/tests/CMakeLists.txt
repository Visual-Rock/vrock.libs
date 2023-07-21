enable_testing()

add_executable(pdf_tests ByteArray.test.cpp)

target_link_libraries(
        pdf_tests PRIVATE
        vrockpdf
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(pdf_tests)