pkg_check_modules(CRYPTOPP IMPORTED_TARGET cryptopp)
pkg_check_modules(LIBCRYPTOPP IMPORTED_TARGET libcryptopp)
pkg_check_modules(LIBCRYPTO++ IMPORTED_TARGET libcrypto++)

add_library(vrocksecurity)
target_sources(vrocksecurity
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        random.ixx
        encryption.ixx
        hash.ixx
        PRIVATE
        random.cpp
        encryption.cpp
        hash.cpp
)

target_link_libraries(vrocksecurity vrockutils)

if (${CRYPTOPP_FOUND})
    target_link_libraries(vrocksecurity PkgConfig::CRYPTOPP)
elseif (${LIBCRYPTOPP_FOUND})
    target_link_libraries(vrocksecurity PkgConfig::LIBCRYPTOPP)
elseif (${LIBCRYPTO++_FOUND})
    target_link_libraries(vrocksecurity PkgConfig::LIBCRYPTO++)
endif ()