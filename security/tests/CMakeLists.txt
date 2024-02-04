enable_testing()

# hash/MD5.test.cpp hash/SHA2.test.cpp
add_executable(security_tests
        encryption/aes.test.cpp
        encryption/rc4.test.cpp
)

target_link_libraries(
        security_tests PRIVATE
        vrocksecurity
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(security_tests)