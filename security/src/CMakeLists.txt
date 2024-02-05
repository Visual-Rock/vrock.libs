add_library(vrocksecurity)
add_library(vrock::security ALIAS vrocksecurity)


find_package(cryptopp CONFIG REQUIRED)

target_include_directories(vrocksecurity PUBLIC include)
target_sources(vrocksecurity PRIVATE
        src/random.cpp
        src/encryption.cpp
        src/hash.cpp
)

target_link_libraries(vrocksecurity PUBLIC vrock::utils)
target_link_libraries(vrocksecurity PRIVATE cryptopp::cryptopp)