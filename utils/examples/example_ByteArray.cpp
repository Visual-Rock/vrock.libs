#include <iostream>

import vrock.utils.ByteArray;
using namespace vrock::utils;

int main( )
{
    ByteArray data( "alksd" );
    ByteArray data1( "alksd" );

    if ( data == data1 )
        std::cout << "equal" << std::endl;

    auto subarr = data.subarr(1, 2);

    return 0;
}