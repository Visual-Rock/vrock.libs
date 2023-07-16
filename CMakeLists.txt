cmake_minimum_required(VERSION 3.26)
project(vrocklibs CXX)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
# Default to C++ extensions being off. Clang's modules support have trouble
# with extensions right now and it is not required for any other compiler
set(CMAKE_CXX_EXTENSIONS OFF)

option(TESTS "Build all vrock.lib Tests" ON)
option(EXAMPLES "Build all vrock.lib Examples" ON)

option(SECURITY "Build the security module of vrock.lib" ON)

include(FindPkgConfig)
find_package(PkgConfig)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    include(gcc_modules.cmake)
endif()
set(CMAKE_CXX_STANDARD 20)

if (${TESTS})
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif ()

add_subdirectory(utils) # utils is always included

if (${SECURITY})
    add_subdirectory(security)
endif ()