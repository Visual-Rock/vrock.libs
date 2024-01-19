#include <iomanip>
#include <iostream>
#include <vrock/security.hpp>

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    std::string data =  "Test";
    std::string key; // Keys have to be either 128, 192, or 256
    key.resize( 32 );
    memset( key.data(  ), 0, key.length(  ) );
    std::string iv;
    iv.resize( 12 );
    memset( iv.data(  ), 0, 12 );
    std::string aad;
    aad.resize( 16 );
    memset( aad.data(  ), 0, 16 );

    auto encrypted = encrypt_aes_gcm( data, key, iv, aad );
    std::cout << "Encrypted: ";
    for ( size_t i = 0; i < encrypted.size( ); ++i )
        std::cout << std::setw( 2 ) << std::setfill( '0' ) << std::hex << (int)encrypted[ i ] << ' ';
    std::cout << std::endl;

    auto decrypt = std::string(encrypted.begin(  ), encrypted.end());
    auto decrypted = decrypt_aes_gcm( decrypt, key, iv, aad );
    std::string tmp(decrypted.begin(), decrypted.end());
    std::cout << "Unencrypted: " << tmp << std::endl;

    return 0;
}