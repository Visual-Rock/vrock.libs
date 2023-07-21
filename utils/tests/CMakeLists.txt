enable_testing()

add_executable(utils_tests ByteArray.test.cpp)

target_link_libraries(
        utils_tests PRIVATE
        vrockutils
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(utils_tests)