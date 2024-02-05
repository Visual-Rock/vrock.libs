add_library(vrockutils)
add_library(vrock::utils ALIAS vrockutils)

target_include_directories(vrockutils PUBLIC ./include/)
target_sources(vrockutils PRIVATE src/ByteArray.cpp)