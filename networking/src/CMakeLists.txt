add_library(vrocknetworking)
target_sources(vrocknetworking
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        Sockets.ixx
        Server.ixx
)
target_link_libraries(vrocknetworking PRIVATE vrockutils)