add_executable(example_BasicLogging example_BasicLogging.cpp)
target_link_libraries(example_BasicLogging PRIVATE vrock::log vrock::utils)