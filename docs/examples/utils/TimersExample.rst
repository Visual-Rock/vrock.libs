.. _examples_utils_timers:

Timer Examples
==============

The Normal Timer
----------------

In modern software development, understanding the performance of various sections of code is crucial for optimization. 
The :ref:`Timer<api_utils_timer>` class presented here offers a convenient way to measure the elapsed time between different points in a program,
aiding in the profiling and analysis of code execution.

The :ref:`Timer<api_utils_timer>` class utilizes the **<chrono>** library in C++ to capture high-resolution time points.
This example demonstrates the usage of the :ref:`Timer<api_utils_timer>` class by measuring the time it takes to perform simulated computational work at two distinct intervals within the program.

.. code-block:: c++
    :caption: using the :ref:`Timer<api_utils_timer>` class

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
    
        return 0;
    }

1. Initialization:
    * A :ref:`Timer<api_utils_timer>` object is instantiated at the beginning of the **main** function. The Timer starts with instantiation  
2. First Work Interval:
    * Simulated work is performed using the **std::this_thread::sleep_for** function, taking 1000 milliseconds (1 second).
    * The elapsed time since the :ref:`Timer<api_utils_timer>` instantiation is then printed.
3. Reset and Second Work Interval:
    * The :ref:`Timer<api_utils_timer>` is reset for a new measurement.
    * Additional simulated work is performed, taking 500 milliseconds (0.5 seconds).
    * The elapsed time since the reset is printed.

Feel free to experiment with different work durations to observe changes in elapsed time, providing insights into the efficiency of your code.

The Scoped Timer
----------------

The :ref:`ScopedTimer<api_utils_scopedtimer>` class goes beyond simple elapsed time calculation; it allows the execution of custom functions upon destruction. This feature is particularly useful for scenarios where cleanup or reporting tasks need to be performed automatically after a specific code block.

.. code-block:: c++
    :caption: using the :ref:`ScopedTimer<api_utils_scopedtimer>` class

    #include <iostream>
    #include <thread>

    #include <vrock/utils.hpp>
    
    using namespace vrock::utils;

    int main( )
    {
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

1. The :ref:`ScopedTimer<api_utils_scopedtimer>` class is instantiated in two different scopes, each with a lambda function provided as a parameter.
2. The lambda functions are executed when the :ref:`ScopedTimer<api_utils_scopedtimer>` objects go out of scope (i.e., at the end of the respective scopes).
3. Inside the lambda functions, the elapsed time since the :ref:`ScopedTimer<api_utils_scopedtimer>` instantiation is obtained and printed.

This example demonstrates how the :ref:`ScopedTimer<api_utils_scopedtimer>` class can be used to conveniently measure the time taken by different tasks within specific scopes, and execute custom cleanup or reporting functions when the tasks complete.