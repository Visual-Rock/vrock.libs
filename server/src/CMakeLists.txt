add_library(vrockserver)
target_sources(vrockserver
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ByteArray.ixx
        Timer.ixx
        FutureHelper.ixx
        List.ixx
)