#include <iomanip>
#include <iostream>

import vrock.utils.ByteArray;
import vrock.security.encryption;

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    auto data = ByteArray<>( "Test" );
    auto key = from_hex_string(
        "0000000000000000000000000000000000000000000000000000000000000000" ); // Keys have to be either 128, 192, or 256
                                                                              // bit long
    auto iv = from_hex_string( "000000000000000000000000" );                  // 12 byte long
    auto aad = from_hex_string( "00000000000000000000000000000000" );

    auto encrypted = encrypt_aes_gcm( data, key, iv, aad );
    std::cout << "Encrypted: ";
    for ( size_t i = 0; i < encrypted.size( ); ++i )
        std::cout << std::setw( 2 ) << std::setfill( '0' ) << std::hex << (int)encrypted[ i ] << ' ';
    std::cout << std::endl;

    auto decrypted = decrypt_aes_gcm( encrypted, key, iv, aad );
    std::cout << "Unencrypted: " << decrypted.to_string( ) << std::endl;

    return 0;
}