.. _examples_utils_generator:

Generator Examples
==================

In this example, the Generator class is used to create a generator function generateNumbers that yields integers from 0 to 4.
The generator is then iterated over using a range-based for loop, printing each value to the console.
The co_yield statement is used to yield values in the generator coroutine.

.. note::
    if available use std::generator

.. code-block:: c++
    :caption: using the :ref:`Task<api_utils_task>` class

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

.. code-block:: console
    :caption: output

    0 1 2 3 4