.. _examples_log_basic_example:

Basic Examples
==============

The following example demonstrates how to use the :ref:`Logger<api_log_logger>` class to log messages to the console.

.. code-block:: c++
    :caption: using the :ref:`Logger<api_log_logger>` class

    #include <vrock/log.hpp>
    using namespace vrock::log;

    int main()
    {
        auto logger = make_logger("example");
        logger->add_sink(std::make_shared<StandardOutSink>());
        logger->info("Hello, World!");

        return 0;
    }

The output of the above example will be something like this:
.. code-block:: console
    :caption: output

    [ 02/03/24 16:53:06 ] [ example ] [ info  ] Hello, World!

It is also possible to get the logger instance using the :ref:`get_logger<api_log_logger>` function.

.. code-block:: c++
    :caption: using the :ref:`get_logger<api_log_logger>` function

    #include <vrock/log.hpp>
    using namespace vrock::log;

    int main()
    {
        auto logger = get_logger("example");
        logger->info("Hello, World!");

        return 0;
    }

.. note::
    if the logger is not created before calling the :ref:`get_logger<api_log_logger>` function, a new logger will be created with the given name and a StandardOutSink.
    It is possible to suppress the creation of the StandardOutSink by setting vrock::log::add_standard_out_to_default to false.

The output of the above example will be something like this:
.. code-block:: console
    :caption: output

    [ 02/03/24 16:53:06 ] [ example ] [ info  ] Hello, World!

It is also possible to change the log level of the logger and the log pattern of the logger. The minimum log level is set to LogLevel::Info and the log pattern is set to "[%Y-%m-%d %H:%M:%S] [%n] [%l] %v"
possible flags for the log pattern are found :ref:`here<pages_log_formatting>`.

.. code-block:: c++
    :caption: changing the log level and the log pattern

    #include <vrock/log.hpp>
    using namespace vrock::log;

    int main()
    {
        auto logger = make_logger("example", LogLevel::Debug, false, "[%Y-%m-%d %H:%M:%S] [%n] [%l] %v");
        logger->add_sink(std::make_shared<StandardOutSink>());
        logger->debug("Hello, World!");

        return 0;
    }

The output of the above example will be something like this:
.. code-block:: console
    :caption: output

    [2024-02-03 17:04:25] [example] [debug] Hello, World!