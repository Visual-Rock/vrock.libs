#include <iostream>

#include <vrock/security.hpp>
#include <vrock/utils.hpp>
using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    auto data = generate_random_bytes( 10 );
    std::cout << to_hex_string( data ) << std::endl;
    return 0;
}