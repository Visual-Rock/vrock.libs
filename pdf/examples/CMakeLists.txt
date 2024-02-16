file(COPY ../pdfs/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
add_executable(example_001 example_001.cpp)
target_link_libraries(example_001 PRIVATE vrockpdf)

find_package(blend2d CONFIG REQUIRED)
add_executable(example_renderer example_renderer.cpp)
target_link_libraries(example_renderer PRIVATE vrockpdf blend2d::blend2d)

