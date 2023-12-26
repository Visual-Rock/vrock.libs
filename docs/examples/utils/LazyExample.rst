.. _examples_utils_lazy:

Lazy Examples
==============

Simple Example
--------------

In this example, the :ref:`Lazy<api_utils_lazy>` class is used to lazily evaluate a value through the lambda expression.
The value is only calculated when it is accessed for the first time, and subsequent accesses reuse the already calculated value.
The output of this program should demonstrate the lazy evaluation behavior.

.. code-block:: c++
    :caption: using the :ref:`Lazy<api_utils_lazy>` class

    #include <iostream>
    #include <vrock/utils.hpp>

    using namespace vrock::utils;

    int main( )
    {
        auto num = Lazy<int>( [] {
            std::cout << "the answer to everything ";
            return 42;
        } );

        std::cout << "num is " << num( ) << std::endl;
        std::cout << "num does not need to be recalculated and is still " << num( ) << std::endl;
    }

.. code-block:: console
    :caption: output

    num is the answer to everything 42
    num does not need to be recalculated and is still 42

1. An instance of the :ref:`Lazy<api_utils_lazy>` class named num is created with a lambda function inside the constructor for lazy evaluation.
The lambda function prints a message and returns the value 42.

2. The lazily evaluated value is accessed using the function call operator num() and printed to the console.

3. Since the value is already calculated during the first access, accessing it again doesn't trigger recalculation.