#pragma once

#include <functional>

namespace vrock::utils
{
    /**
     * @brief A class template for lazy evaluation of a function.
     * @tparam T The type of the value to be lazily evaluated.
     *
     * @attention if the is not loaded but assigned to it is marked as loaded
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
        explicit Lazy( std::function<T( )> fn ) noexcept : _fn( std::move( fn ) )
        {
        }

        /**
         * @brief Copy constructor for Lazy.
         * @param lazy The Lazy object to be copied.
         */
        Lazy( const Lazy &lazy ) noexcept : _loaded( lazy._loaded )
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
         * @brief get method for lazy evaluation.
         * @return A reference to the lazily evaluated value.
         */
        auto get( ) noexcept -> T &
        {
            if ( !_loaded )
            {
                _value = _fn( );
                _loaded = true;
            }
            return _value;
        }

        /**
         * @brief Checks if the Lazy object has a loaded value.
         * @return `true` if a value is already loaded, `false` otherwise.
         */
        [[nodiscard]] auto is_loaded( ) const noexcept -> bool
        {
            return _loaded;
        }

        /**
         * @brief Function call operator for lazy evaluation.
         * @return A reference to the lazily evaluated value.
         */
        T &operator( )( ) noexcept
        {
            return get( );
        }

        /**
         * @brief Access operator for lazy evaluation, providing a reference to the lazily evaluated value.
         * @return A reference to the lazily evaluated value.
         */
        T &operator*( ) noexcept
        {
            return get( );
        }

        /**
         * @brief Copy assignment operator for Lazy.
         * @param lazy The Lazy object to be assigned.
         * @return Reference to the assigned Lazy object.
         */
        Lazy &operator=( const Lazy &lazy ) noexcept
        {
            if ( lazy._loaded )
                _value = lazy._value;
            else
                _fn = lazy._fn;
            _loaded = lazy._loaded;
            return *this;
        }

        /**
         * @brief Copy assignment operator for Lazy, assigning a value directly.
         * @param val The value to be assigned.
         * @return Reference to the assigned Lazy object.
         */
        Lazy &operator=( const T &val ) noexcept
        {
            _loaded = true;
            _value = val;
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

        /**
         * @brief Move assignment operator for Lazy, assigning a value directly.
         * @param val The value to be assigned.
         * @return Reference to the moved Lazy object.
         */
        Lazy &operator=( T &&val ) noexcept
        {
            _loaded = true;
            _value = std::move( val );
            return *this;
        }

    private:
        bool _loaded = false;    ///< Flag indicating whether the value is already loaded.
        std::function<T( )> _fn; ///< The function for lazy evaluation.
        T _value;                ///< The lazily evaluated value.
    };
} // namespace vrock::utils
