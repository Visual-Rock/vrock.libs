#include <iomanip>
#include <iostream>
#include <vrock/security.hpp>

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    auto data = ByteArray<>( "Test" );
    std::cout << "hash: " << md5( data ).to_hex_string( ) << std::endl
              << "ref:  0cbc6611f5540bd0809a388dc95a615b" << std::endl;
}