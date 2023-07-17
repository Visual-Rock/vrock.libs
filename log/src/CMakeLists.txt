if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

add_library(vrocklog)
target_sources(vrocklog
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        Logger.ixx
        PRIVATE
        Logger.cpp
)
target_link_libraries(vrocklog vrockutils spdlog::spdlog)