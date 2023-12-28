#include <iostream>

#include <vrock/utils.hpp>

using namespace vrock::utils;

int main( )
{
    // Define a generator that produces a sequence of integers from 0 to 4
    auto generateNumbers = []( ) -> Generator<int> {
        for ( int i = 0; i < 5; ++i )
        {
            co_yield i; // Using co_yield to yield values in a coroutine
        }
    };

    // Iterate over the generator using a range-based for loop
    for ( const int value : generateNumbers( ) )
    {
        std::cout << value << " ";
    }

    std::cout << std::endl;

    return 0;
}