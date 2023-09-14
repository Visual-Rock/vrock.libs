add_library(vrocksecurity)
target_sources(vrocksecurity
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        random.ixx
        encryption.ixx
        hash.ixx
        PRIVATE
        random.cpp
        encryption.cpp
        hash.cpp
)

target_link_libraries(vrocksecurity vrockutils cryptopp)