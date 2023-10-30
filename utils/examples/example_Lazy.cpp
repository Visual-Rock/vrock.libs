#include <iostream>
#include <vrock/utils.hpp>

using namespace vrock::utils;

int main( )
{
    Lazy<int> num = Lazy<int>( []( ) {
        std::cout << "the answer to everything ";
        return 42;
    } );

    std::cout << "num is " << num( ) << std::endl;
    std::cout << "num does not need to be recalculated and is still " << num( ) << std::endl;
}