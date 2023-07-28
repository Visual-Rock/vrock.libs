FetchContent_Declare(llhttp
        URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz")

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
FetchContent_MakeAvailable(llhttp)

add_library(vrockhttp)
target_sources(vrockhttp
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        TCPServer.ixx
        HttpServer.ixx
        HttpMessage.ixx
        HttpParser.ixx
        Sockets.ixx
        Path.ixx

        PRIVATE
        llhttp/llhttp.c
        llhttp/http.c
        llhttp/api.c
)
target_link_libraries(vrockhttp PRIVATE vrockutils llhttp::llhttp)
target_include_directories(vrockhttp PRIVATE ${LLHTTP_HEADERS})