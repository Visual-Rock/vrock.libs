cmake_minimum_required(VERSION 3.26)
project(vrocklibs)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
if (${CMAKE_MINOR_VERSION} EQUAL 26)
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
endif ()
if (${CMAKE_MINOR_VERSION} EQUAL 27)
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")
endif ()
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
# Default to C++ extensions being off. Clang's modules support have trouble
# with extensions right now and it is not required for any other compiler
set(CMAKE_CXX_EXTENSIONS OFF)

option(TESTS "Build all vrock.lib Tests" OFF)
option(EXAMPLES "Build all vrock.lib Examples" OFF)
option(DOCS "Build vrock.lib Docs" ON)

option(SECURITY "Build the security module of vrock.lib" ON)
option(LOG "Build the log module of vrock.lib" ON)
option(UI "Build the ui module of vrock.lib" ON)
option(PDF "Build the pdf module of vrock.lib" ON)

include(FindPkgConfig)
find_package(PkgConfig)

add_subdirectory(external)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(CONCAT CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE
            "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE>"
            " -MT <DYNDEP_FILE> -MD -MF <DEP_FILE>"
            " ${flags_to_scan_deps} -fdep-file=<DYNDEP_FILE> -fdep-output=<OBJECT>"
    )

    set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FORMAT "gcc")
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FLAG
            "${compiler_flags_for_module_map} -fmodule-mapper=<MODULE_MAP_FILE>")
endif ()
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

if (${LOG})
    add_subdirectory(log)
endif ()

if (${UI})
    add_subdirectory(ui)
endif ()

if (${PDF})
    add_subdirectory(pdf)
endif ()

if (${DOCS})
    add_subdirectory(docs)
endif ()