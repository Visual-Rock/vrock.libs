#include <vrock/utils.hpp>

#include <gtest/gtest.h>

using namespace vrock::utils;

auto async_function( ) -> Task<int>
{
    co_return 42;
}

// Test case for basic Task functionality
TEST( TaskTest, BasicFunctionality )
{
    // Create a Task for an asynchronous operation
    Task<int> asyncTask = async_function( );

    // Ensure the task is not ready initially
    EXPECT_FALSE( asyncTask.is_ready( ) );

    // Access the result using co_await
    int result = await( asyncTask );

    // Ensure the task is ready after resuming
    EXPECT_TRUE( asyncTask.is_ready( ) );

    // Check if the result matches the expected value
    EXPECT_EQ( result, 42 );
}