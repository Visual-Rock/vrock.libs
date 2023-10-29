module;

#include <coroutine>
#include <exception>
#include <iterator>
#include <type_traits>

export module vrock.utils:Generator;

namespace vrock::utils
{
    export template <typename T>
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

    template <typename T>
    class Generator
    {
    public:
        using promise_type = GeneratorPromise<T>;
        using iterator = GeneratorIterator<T>;
        using sentinel = GeneratorSentinel;

        Generator( ) noexcept : _coroutine( nullptr )
        {
        }

        Generator( const Generator & ) = delete;
        Generator( Generator &&other ) noexcept : _coroutine( other._coroutine )
        {
            other._coroutine = nullptr;
        }

        ~Generator( )
        {
            if ( _coroutine )
                _coroutine.destroy( );
        }

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

        auto end( ) noexcept -> sentinel
        {
            return sentinel{ };
        }

        auto operator=( const Generator & ) = delete;
        auto operator=( Generator &&other ) noexcept -> Generator &
        {
            _coroutine = other._coroutine;
            other._coroutine = nullptr;

            return *this;
        }

    private:
        friend class GeneratorPromise<T>;

        explicit Generator( std::coroutine_handle<promise_type> coroutine ) noexcept : _coroutine( coroutine )
        {
        }

        std::coroutine_handle<promise_type> _coroutine;
    };

    template <typename T>
    auto GeneratorPromise<T>::get_return_object( ) noexcept -> Generator<T>
    {
        return Generator<T>{ std::coroutine_handle<GeneratorPromise<T>>::from_promise( *this ) };
    }
} // namespace vrock::utils