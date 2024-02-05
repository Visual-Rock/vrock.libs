enable_testing()

add_executable(log_tests
        Logger.test.cpp

        FlagFormatters/AlignFormatters.test.cpp
        FlagFormatters/GeneralFormatters.test.cpp
        FlagFormatters/AnsiColorFormatters.test.cpp
        FlagFormatters/SourceFormatters.test.cpp
        FlagFormatters/TimeFormatters.test.cpp
)

target_link_libraries(
        log_tests PRIVATE
        vrock::log
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(log_tests)