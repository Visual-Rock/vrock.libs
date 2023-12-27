#pragma once

#include <coroutine>
#include <exception>
#include <iterator>
#include <type_traits>

namespace vrock::utils
{
    template <typename T>
    class Generator;

    template <typename T>
    class GeneratorPromise
    {
    public:
        using value_type = std::remove_reference_t<T>;
        using reference_type = std::conditional_t<std::is_reference_v<T>, T, T &>;
        using pointer_type = value_type *;

        GeneratorPromise( ) = default;

        auto get_return_object( ) noexcept -> Generator<T>;

        auto initial_suspend( ) const
        {
            return std::suspend_always{ };
        }

        auto final_suspend( ) const noexcept( true )
        {
            return std::suspend_always{ };
        }

        template <typename U = T, std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
        auto yield_value( std::remove_reference_t<T> &value ) noexcept
        {
            _value = std::addressof( value );
            return std::suspend_always{ };
        }

        auto yield_value( std::remove_reference_t<T> &&value ) noexcept
        {
            _value = std::addressof( value );
            return std::suspend_always{ };
        }

        auto unhandled_exception( ) -> void
        {
            _exception = std::current_exception( );
        }

        auto return_void( ) noexcept -> void
        {
        }

        auto value( ) const noexcept -> reference_type
        {
            return static_cast<reference_type>( *_value );
        }

        template <typename U>
        auto await_transform( U &&value ) -> std::suspend_never = delete;

        auto rethrow_if_exception( ) -> void
        {
            if ( _exception )
            {
                std::rethrow_exception( _exception );
            }
        }

    private:
        pointer_type _value{ nullptr };
        std::exception_ptr _exception;
    };

    struct GeneratorSentinel
    {
    };

    template <typename T>
    class GeneratorIterator
    {
        using coroutine_handle = std::coroutine_handle<GeneratorPromise<T>>;

    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = typename GeneratorPromise<T>::value_type;
        using reference = typename GeneratorPromise<T>::reference_type;
        using pointer = typename GeneratorPromise<T>::pointer_type;

        GeneratorIterator( ) noexcept
        {
        }

        explicit GeneratorIterator( coroutine_handle coroutine ) noexcept : _coroutine( coroutine )
        {
        }

        friend auto operator==( const GeneratorIterator &it, GeneratorSentinel ) noexcept -> bool
        {
            return it._coroutine == nullptr || it._coroutine.done( );
        }

        friend auto operator!=( const GeneratorIterator &it, GeneratorSentinel s ) noexcept -> bool
        {
            return !( it == s );
        }

        friend auto operator==( GeneratorSentinel s, const GeneratorIterator &it ) noexcept -> bool
        {
            return ( it == s );
        }

        friend auto operator!=( GeneratorSentinel s, const GeneratorIterator &it ) noexcept -> bool
        {
            return it != s;
        }

        GeneratorIterator &operator++( )
        {
            _coroutine.resume( );
            if ( _coroutine.done( ) )
            {
                _coroutine.promise( ).rethrow_if_exception( );
            }

            return *this;
        }

        auto operator++( int ) -> void
        {
            (void)operator++( );
        }

        reference operator*( ) const noexcept
        {
            return _coroutine.promise( ).value( );
        }

        pointer operator->( ) const noexcept
        {
            return std::addressof( operator*( ) );
        }

    private:
        coroutine_handle _coroutine{ nullptr };
    };

    /**
     * @brief A generator class template that represents a coroutine-based generator.
     *
     * @tparam T The type of values produced by the generator.
     */
    template <typename T>
    class Generator
    {
    public:
        /**
         * @brief The promise type associated with the generator.
         */
        using promise_type = GeneratorPromise<T>;

        /**
         * @brief Iterator type for the generator.
         */
        using iterator = GeneratorIterator<T>;

        /**
         * @brief Sentinel type for the generator.
         */
        using sentinel = GeneratorSentinel;

        /**
         * @brief Default constructor for the generator.
         */
        Generator( ) noexcept : _coroutine( nullptr )
        {
        }

        /**
         * @brief Deleted copy constructor to prevent copying of generators.
         */
        Generator( const Generator & ) = delete;

        /**
         * @brief Move constructor for the generator.
         *
         * @param other The generator to be moved.
         */
        Generator( Generator &&other ) noexcept : _coroutine( other._coroutine )
        {
            other._coroutine = nullptr;
        }

        /**
         * @brief Destructor for the generator.
         * Destroys the coroutine if it is still active.
         */
        ~Generator( )
        {
            if ( _coroutine )
                _coroutine.destroy( );
        }

        /**
         * @brief Retrieves the iterator pointing to the beginning of the generator sequence.
         * Resumes the coroutine if not done and rethrows any exceptions if present.
         *
         * @return An iterator representing the beginning of the generator sequence.
         */
        auto begin( ) -> iterator
        {
            if ( _coroutine != nullptr )
            {
                _coroutine.resume( );
                if ( _coroutine.done( ) )
                    _coroutine.promise( ).rethrow_if_exception( );
            }
            return iterator{ _coroutine };
        }

        /**
         * @brief Retrieves the sentinel representing the end of the generator sequence.
         *
         * @return A sentinel representing the end of the generator sequence.
         */
        auto end( ) noexcept -> sentinel
        {
            return sentinel{ };
        }

        /**
         * @brief Deleted copy assignment operator to prevent copying of generators.
         */
        auto operator=( const Generator & ) = delete;

        /**
         * @brief Move assignment operator for the generator.
         *
         * @param other The generator to be moved.
         * @return A reference to the moved generator.
         */
        auto operator=( Generator &&other ) noexcept -> Generator &
        {
            _coroutine = other._coroutine;
            other._coroutine = nullptr;

            return *this;
        }

    private:
        friend class GeneratorPromise<T>;

        /**
         * @brief Private constructor used to create a generator from a coroutine handle.
         *
         * @param coroutine The coroutine handle associated with the generator.
         */
        explicit Generator( std::coroutine_handle<promise_type> coroutine ) noexcept : _coroutine( coroutine )
        {
        }

        /**
         * @brief The coroutine handle associated with the generator.
         */
        std::coroutine_handle<promise_type> _coroutine;
    };

    template <typename T>
    auto GeneratorPromise<T>::get_return_object( ) noexcept -> Generator<T>
    {
        return Generator<T>{ std::coroutine_handle<GeneratorPromise<T>>::from_promise( *this ) };
    }
} // namespace vrock::utils