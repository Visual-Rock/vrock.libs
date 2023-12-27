#include <gtest/gtest.h>
#include <vrock/utils.hpp> // Include the header file with your functions

using namespace vrock::utils;

// Test for future_ready function
TEST( FutureReadyTest, ValidFuture )
{
    std::promise<int> promise;
    std::future<int> future = promise.get_future( );

    promise.set_value( 42 );

    EXPECT_TRUE( future_ready( future ) );
}

TEST( FutureReadyTest, InvalidFuture )
{
    std::future<int> future;

    EXPECT_FALSE( future_ready( future ) );
}

// Test for future_timeout function
TEST( FutureTimeoutTest, Timeout )
{
    std::promise<int> promise;
    std::future<int> future = promise.get_future( );

    EXPECT_TRUE( future_timeout( future, std::chrono::milliseconds( 100 ) ) );
}

TEST( FutureTimeoutTest, NotTimeout )
{
    std::promise<int> promise;
    std::future<int> future = promise.get_future( );

    promise.set_value( 42 );

    EXPECT_FALSE( future_timeout( future, std::chrono::milliseconds( 100 ) ) );
}

// Test for future_deferred function
TEST( FutureDeferredTest, Deferred )
{
    std::promise<int> promise;
    std::future<int> future = promise.get_future( );

    EXPECT_FALSE( future_deferred( future ) );
}

TEST( FutureDeferredTest, NotDeferred )
{
    std::promise<int> promise;
    std::future<int> future = promise.get_future( );

    promise.set_value( 42 );

    EXPECT_FALSE( future_deferred( future ) );
}