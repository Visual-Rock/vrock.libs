#pragma once

#include <functional>

namespace vrock::utils
{
    /**
     * @brief A class template for lazy evaluation of a function.
     * @tparam T The type of the value to be lazily evaluated.
     */
    template <typename T>
    class Lazy
    {
    public:
        /**
         * @brief Deleted default constructor.
         */
        Lazy( ) = delete;

        /**
         * @brief Constructor that takes a function for lazy evaluation.
         * @param fn The function to be lazily evaluated.
         */
        explicit Lazy( std::function<T( )> fn ) : _fn( std::move( fn ) )
        {
        }

        /**
         * @brief Copy constructor for Lazy.
         * @param lazy The Lazy object to be copied.
         */
        Lazy( const Lazy &lazy ) : _loaded( lazy._loaded )
        {
            if ( lazy._loaded )
                _value = lazy._value;
            else
                _fn = lazy._fn;
        }

        /**
         * @brief Move constructor for Lazy.
         * @param lazy The Lazy object to be moved.
         */
        Lazy( Lazy &&lazy ) noexcept : _loaded( lazy._loaded )
        {
            if ( lazy._loaded )
                _value = std::move( lazy._value );
            else
                _fn = std::move( lazy._fn );
        }

        /**
         * @brief Function call operator for lazy evaluation.
         * @return A reference to the lazily evaluated value.
         */
        T &operator( )( )
        {
            if ( !_loaded )
            {
                _value = _fn( );
                _loaded = true;
            }
            return _value;
        }

        /**
         * @brief Copy assignment operator for Lazy.
         * @param lazy The Lazy object to be assigned.
         * @return Reference to the assigned Lazy object.
         */
        Lazy &operator=( const Lazy &lazy )
        {
            if ( lazy._loaded )
                _value = lazy._value;
            else
                _fn = lazy._fn;
            _loaded = lazy._loaded;
            return *this;
        }

        /**
         * @brief Move assignment operator for Lazy.
         * @param lazy The Lazy object to be moved.
         * @return Reference to the moved Lazy object.
         */
        Lazy &operator=( Lazy &&lazy ) noexcept
        {
            if ( lazy._loaded )
                _value = std::move( lazy._value );
            else
                _fn = std::move( lazy._fn );
            _loaded = lazy._loaded;
            return *this;
        }

    private:
        bool _loaded = false;    ///< Flag indicating whether the value is already loaded.
        std::function<T( )> _fn; ///< The function for lazy evaluation.
        T _value;                ///< The lazily evaluated value.
    };
} // namespace vrock::utils
