enable_testing()

add_executable(ui_tests main.cpp)

target_link_libraries(
        ui_tests PRIVATE
        vrockui
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ui_tests)