add_subdirectory(src)

if (${EXAMPLES})
    add_subdirectory(examples)
endif ()

if (${TESTS})
    add_subdirectory(tests)
endif ()