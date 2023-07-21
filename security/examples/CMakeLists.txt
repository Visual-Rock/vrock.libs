add_executable(example_Random example_Random.cpp)
target_link_libraries(example_Random PRIVATE vrocksecurity)

add_executable(example_AES example_AES.cpp)
target_link_libraries(example_AES PRIVATE vrocksecurity)

add_executable(example_RC4 example_rc4.cpp)
target_link_libraries(example_RC4 PRIVATE vrocksecurity)

add_executable(example_MD5 example_MD5.cpp)
target_link_libraries(example_MD5 PRIVATE vrocksecurity)

add_executable(example_sha2 example_sha2.cpp)
target_link_libraries(example_sha2 PRIVATE vrocksecurity)