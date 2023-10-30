cmake_minimum_required(VERSION 3.26)
project(vrocklibs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

option(VROCKLIBS_TESTS "Build all vrock.lib Tests" OFF)
option(VROCKLIBS_EXAMPLES "Build all vrock.lib Examples" OFF)
option(VROCKLIBS_DOCS "Build vrock.lib Docs" OFF)

option(VROCKLIBS_SECURITY "Build the security module of vrock.lib" OFF)
option(VROCKLIBS_LOG "Build the log module of vrock.lib" OFF)
option(VROCKLIBS_UI "Build the ui module of vrock.lib" OFF)
option(VROCKLIBS_PDF "Build the pdf module of vrock.lib" OFF)

add_subdirectory(external)

if (${VROCKLIBS_TESTS})
    include(CTest)
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

if (${VROCKLIBS_SECURITY})
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

if (${VROCKLIBS_DOCS})
    add_subdirectory(docs)
endif ()
