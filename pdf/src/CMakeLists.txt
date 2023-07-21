
add_library(vrockpdf)
target_sources(vrockpdf
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        PDFDocument.ixx
)

target_link_libraries(vrockpdf PRIVATE vrockutils vrocksecurity vrocklog)