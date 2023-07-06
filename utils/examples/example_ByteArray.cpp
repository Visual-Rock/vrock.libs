import vrock.utils.ByteArray;
import vrock.utils.List;

#include <iostream>

using namespace vrock::utils;

int main( )
{
    ByteArray data( "alksd" );
    ByteArray data1( 8 );

    auto subarr = data.subarr(1, 2);

    return 0;
}