file(COPY ../pdfs/with_update_sections.pdf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
add_executable(example_001 example_001.cpp)
target_link_libraries(example_001 PRIVATE vrockpdf)