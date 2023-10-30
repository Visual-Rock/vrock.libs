add_library(vrockutils)
target_include_directories(vrockutils PUBLIC ./include/)
target_sources(vrockutils PRIVATE src/ByteArray.cpp)