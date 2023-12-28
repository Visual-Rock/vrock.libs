#include <iostream>
#include <thread>
#include <vrock/utils.hpp>

using namespace vrock::utils;

int main( )
{
    // Create a Timer object
    Timer timer;

    // Perform some work (e.g., processing data, calculations)
    std::cout << "Doing some work..." << std::endl;
    // Simulating work that takes 1000 milliseconds (1 second)
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    // Get and print the elapsed time since the Timer was started
    auto elapsedTime = timer.elapsed( );
    std::cout << "Elapsed time: " << elapsedTime << " milliseconds" << std::endl;

    // Reset the Timer for a new measurement
    timer.reset( );

    // Perform more work (e.g., additional computations)
    std::cout << "Doing more work..." << std::endl;
    // Simulating work that takes 500 milliseconds (0.5 seconds)
    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

    // Get and print the elapsed time again since the Timer was reset
    elapsedTime = timer.elapsed( );
    std::cout << "Elapsed time: " << elapsedTime << " milliseconds" << std::endl;

    // Example 1: ScopedTimer with default function on destruction
    {
        ScopedTimer scopedTimer( []( const std::uint64_t elapsed ) {
            std::cout << "Task 1 took " << elapsed << " milliseconds." << std::endl;
        } );

        // Simulate Task 1
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
    } // The ScopedTimer goes out of scope, and the elapsed time is printed

    // Example 2: ScopedTimer with a custom function on destruction
    {
        ScopedTimer scopedTimer( []( const std::uint64_t elapsed ) {
            std::cout << "Task 2 took " << elapsed << " milliseconds and completed some cleanup." << std::endl;
        } );

        // Simulate Task 2
        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    } // The ScopedTimer goes out of scope, and the elapsed time is printed along with cleanup message

    return 0;
}