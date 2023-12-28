#include <iostream>
#include <vrock/utils.hpp>

using namespace vrock::utils;

int main( )
{
    // Creating a Lazy object of type int with a lambda function for lazy evaluation
    auto num = Lazy<int>( [] {
        std::cout << "the answer to everything ";
        return 42;
    } );

    // Accessing and printing the lazily evaluated value
    std::cout << "num is " << *num << std::endl;
    // Since the value is already calculated, it doesn't need to be recalculated
    std::cout << "num does not need to be recalculated and is still " << num( ) << std::endl;

    // Changing the value of a lazy
    num = 32;
    // when you now retrieve the value the new value is returned
    std::cout << "the changed value is " << num.get( ) << std::endl;

    return 0;
}