find_package(fmt CONFIG REQUIRED)

add_library(vrocklog)
target_include_directories(vrocklog PUBLIC ./include/)
target_sources(vrocklog PRIVATE src/Logger.cpp)

target_link_libraries(vrocklog PUBLIC fmt::fmt)