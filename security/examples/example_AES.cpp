#include <iostream>

#include <vrock/security.hpp>
#include <vrock/utils.hpp>

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    const std::string data = "Test";
    const std::string key( 32, '\0' ); // Keys have to be either 128, 192, or 256
    const std::string iv( 12, '\0' );  // IVs have to be 12 bytes long (96 bits)
    const std::string aad( 16, '\0' ); // AADs have to be 16 bytes long (128 bits)

    // encrypt the data
    auto encrypted = encrypt_aes_gcm( data, key, iv, aad );
    std::cout << "Encrypted: " << to_hex_string( encrypted ) << std::endl;

    // decrypt the encrypted data
    auto decrypted = decrypt_aes_gcm( encrypted, key, iv, aad );
    std::cout << "Unencrypted: " << decrypted << std::endl;

    return 0;
}