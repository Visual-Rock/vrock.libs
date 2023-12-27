.. _api_utils_future_helper:

Future Helper
=========

.. note::
    every future_\* has two signatures one for std::future and one for std::shared_future

Ready
-----

.. doxygenfunction:: vrock::utils::future_ready( const std::future<T> &future, std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) )
    :project: vrock.libs

.. doxygenfunction:: vrock::utils::future_ready( const std::shared_future<T> &future, std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) )
    :project: vrock.libs

Timeout
-------

.. doxygenfunction:: vrock::utils::future_timeout( const std::future<T> &future, std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) )
    :project: vrock.libs

.. doxygenfunction:: vrock::utils::future_timeout( const std::shared_future<T> &future, std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) )
    :project: vrock.libs

Deferred
--------

.. doxygenfunction:: vrock::utils::future_deferred( const std::future<T> &future, std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) )
    :project: vrock.libs

.. doxygenfunction:: vrock::utils::future_deferred( const std::shared_future<T> &future, std::chrono::milliseconds duration = std::chrono::milliseconds( 0 ) )
    :project: vrock.libs