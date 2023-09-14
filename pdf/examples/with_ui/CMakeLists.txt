add_executable(Example_PdfUi)
target_sources(Example_PdfUi
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        Globals.ixx
        RenderHelper.ixx
        DocumentStructureWidget.ixx
        DocumentViewWidget.ixx

        PRIVATE
        main.cpp
)
target_link_libraries(Example_PdfUi PRIVATE vrockpdf vrockui)