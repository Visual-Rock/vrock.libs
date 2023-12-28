.. _api_utils_coroutine_helper:

Coroutine Helper
================

.. note::
    the returned when_all task needs to be awaited:
    **await( when_all( ...) );**

.. doxygenfunction:: vrock::utils::await
    :project: vrock.libs

.. doxygenfunction:: vrock::utils::when_all( AwaitablesType... awaitables )
    :project: vrock.libs

.. doxygenfunction:: vrock::utils::when_all( RangeType awaitables )
    :project: vrock.libs