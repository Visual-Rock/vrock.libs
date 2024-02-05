add_executable(example_Random example_Random.cpp)
target_link_libraries(example_Random PRIVATE vrock::security)

add_executable(example_AES example_AES.cpp)
target_link_libraries(example_AES PRIVATE vrock::security)

add_executable(example_RC4 example_rc4.cpp)
target_link_libraries(example_RC4 PRIVATE vrock::security)

add_executable(example_MD5 example_MD5.cpp)
target_link_libraries(example_MD5 PRIVATE vrock::security)

add_executable(example_sha2 example_sha2.cpp)
target_link_libraries(example_sha2 PRIVATE vrock::security)