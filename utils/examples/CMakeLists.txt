add_executable(example_ByteArray example_ByteArray.cpp)
target_link_libraries(example_ByteArray PRIVATE vrock::utils)

add_executable(example_Coroutines example_Coroutines.cpp)
target_link_libraries(example_Coroutines PRIVATE vrock::utils)

add_executable(example_Generator example_Generator.cpp)
target_link_libraries(example_Generator PRIVATE vrock::utils)

add_executable(example_Task example_Task.cpp)
target_link_libraries(example_Task PRIVATE vrock::utils)


add_executable(example_Lazy example_Lazy.cpp)
target_link_libraries(example_Lazy PRIVATE vrock::utils)

add_executable(example_Timer example_Timer.cpp)
target_link_libraries(example_Timer PRIVATE vrock::utils)