find_package(OpenSSL REQUIRED)

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
        Interceptor.ixx

        PRIVATE
        llhttp/llhttp.c
        llhttp/http.c
        llhttp/api.c
)
target_link_libraries(vrockhttp PRIVATE vrockutils OpenSSL::SSL OpenSSL::Crypto)