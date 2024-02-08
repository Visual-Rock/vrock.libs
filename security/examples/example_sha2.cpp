#include <iostream>

#include <vrock/security.hpp>
#include <vrock/utils/SpanHelpers.hpp>

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    auto data = "Test";

    std::cout << "SHA-224: " << to_hex_string( sha224( data ) ) << std::endl
              << "ref:     3606346815fd4d491a92649905a40da025d8cf15f095136b19f37923" << std::endl;

    std::cout << "SHA-256: " << to_hex_string( sha256( data ) ) << std::endl
              << "ref:     532eaabd9574880dbf76b9b8cc00832c20a6ec113d682299550d7a6e0f345e25" << std::endl;

    std::cout
        << "SHA-384: " << to_hex_string( sha384( data ) ) << std::endl
        << "ref:     7b8f4654076b80eb963911f19cfad1aaf4285ed48e826f6cde1b01a79aa73fadb5446e667fc4f90417782c91270540f3"
        << std::endl;

    std::cout << "SHA-512: " << to_hex_string( sha512( data ) ) << std::endl
              << "ref:     "
                 "c6ee9e33cf5c6715a1d148fd73f7318884b41adcb916021e2bc0e800a5c5dd97f5142178f6ae88c8fdd98e1afb0ce4c8d2c54"
                 "b5f37b30b7da1997bb33b0b8a31"
              << std::endl;
}