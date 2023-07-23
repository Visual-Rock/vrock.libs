enable_testing()

add_executable(server_tests ByteArray.test.cpp)

target_link_libraries(
        server_tests PRIVATE
        vrockserver
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(server_tests)