How to use
==========

vrock libs use the cmake system so integrating it into your cmake project is fairly easy.
you can add it as submodule.

.. code-block::

    git submodule add https://github.com/Visual-Rock/vrock.libs.git <path>

now you can add the subdirectory and add the dependency
to your Binary

.. code-block:: cmake
    :caption: adding as dependency

    add_subdirectory(<path>/vrock.libs)

    target_link_libraries(TARGET PUBLIC vrockutils)

To include the :ref:`Timer<api_utils_timer>` class do the following

.. code-block:: c++
    :caption: adding ByteArray module

    #include <vrock/utils.hpp>

    int main( )
    {
        auto array = vrock::utils::Timer( );
    }
