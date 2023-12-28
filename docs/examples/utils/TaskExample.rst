.. _examples_utils_task:

Task Examples
=============

In this example we use the :ref:`Task<api_utils_task>` class, which is a generic class template representing an
asynchronous task using C++20 coroutines. The :ref:`Task<api_utils_task>` class is utilized in a simple asynchronous
function, `async_function`, which returns a :ref:`Task\<int\><api_utils_task>`. Another asynchronous function,
`print_result_async`, awaits the result of `async_function` and prints the value.
The main function awaits the task.

.. note::
    it is recommended to use the :ref:`await<api_utils_coroutine_helper>` and
    :ref:`when_all<api_utils_coroutine_helper>` methods.
    for more information look at the coroutine example in the utils example folder

.. code-block:: c++
    :caption: using the :ref:`Task<api_utils_task>` class

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

.. code-block:: console
    :caption: output

    Async Function Result: 42

* The async_function is a simple asynchronous function returning a :ref:`Task\<int\><api_utils_task>`.
* The print_result_async function asynchronously awaits the result of async_function and prints it.
* In the main function, an instance of :ref:`Task\<void\><api_utils_task>` is created for the print_result_async operation, and the task is manually resumed until it is ready.
