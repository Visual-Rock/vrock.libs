#include <iostream>

#include <vrock/security.hpp>
#include <vrock/utils/SpanHelpers.hpp>

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    auto data = "Plaintext";
    auto key = "Key";

    auto encrypted = encrypt_rc4( data, key );
    std::cout << "Encrypted: " << to_hex_string( encrypted ) << std::endl;
    std::cout << "Decrypted: " << decrypt_rc4( encrypted, key ) << std::endl;
}
