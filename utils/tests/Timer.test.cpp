#include <vrock/utils.hpp>

#include <gtest/gtest.h>

using namespace vrock::utils;

TEST( TimerTest, BasicAssertion )
{
    Timer timer;

    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    EXPECT_GE( timer.elapsed( ), 1000 );
    timer.reset( );
    EXPECT_LT( timer.elapsed( ), 1000 );
}

TEST( ScopedTimerTest, BasicAssertion )
{
    ScopedTimer timer( []( auto t ) { EXPECT_GE( t, 1000 ); } );

    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
}