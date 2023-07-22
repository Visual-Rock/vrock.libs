enable_testing()

add_executable(ui_tests ByteArray.test.cpp)

target_link_libraries(
        ui_tests PRIVATE
        vrockui
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ui_tests)