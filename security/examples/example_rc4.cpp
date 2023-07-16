#include <iomanip>
#include <iostream>

import vrock.utils.ByteArray;
import vrock.security.encryption;

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    auto data = vrock::utils::ByteArray<>( "Plaintext" );
    auto key = vrock::utils::ByteArray<>( "Key" );

    auto encrypted = encrypt_rc4( data, key );

    for ( size_t i = 0; i < encrypted.size(); ++i )
        std::cout << std::setw( 2 ) << std::setfill( '0' ) << std::hex << (int)encrypted[ i ] << ' ';
    std::cout << std::endl;

    std::cout << decrypt_rc4( encrypted, key ).to_string( ) << std::endl;
}
