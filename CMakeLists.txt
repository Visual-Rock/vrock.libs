cmake_minimum_required(VERSION 3.26)
project(vrock_libs)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)
include_directories(utils)
include_directories(utils/examples)
include_directories(utils/src)

add_executable(vrock_libs
        utils/examples/example_ByteArray.cpp
        utils/src/ByteArray.ixx
        utils/src/List.ixx
)
