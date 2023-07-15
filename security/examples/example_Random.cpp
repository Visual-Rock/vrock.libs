#include <iostream>

import vrock.security.random;
using namespace vrock::security;

int main( )
{
    auto data = generate_random_bytes( 10 );
    for ( size_t i = 0; i < data.size( ); i++ )
        std::cout << data[ i ];
    std::cout << std::endl; 
    return 0;
}