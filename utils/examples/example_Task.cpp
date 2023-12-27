#include <iostream>
#include <vrock/utils.hpp>

using namespace vrock::utils;

// A simple asynchronous function returning a Task.
auto async_function( ) -> Task<int>
{
    co_return 42;
}

// A function that asynchronously awaits a Task and prints the result.
auto print_result_async( ) -> Task<>
{
    std::cout << "Async Function Result: " << co_await async_function( ) << std::endl;
    co_return;
}

int main( )
{
    // Create a Task for the asynchronous operation
    Task<> asyncTask = print_result_async( );

    // Check if the task is ready
    await( asyncTask );

    return 0;
}