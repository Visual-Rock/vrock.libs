add_library(vrockutils)
target_sources(vrockutils
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES

        utils.ixx

        ByteArray.ixx
        Timer.ixx
        FutureHelper.ixx
        List.ixx
        Task.ixx
        Concepts.ixx
)