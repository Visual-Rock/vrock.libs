add_subdirectory(src)

if (${VROCKLIBS_EXAMPLES})
    add_subdirectory(examples)
endif ()

if (${TESTS})
    add_subdirectory(tests)
endif ()