module;

#include <coroutine>
#include <cstdint>
#include <exception>
#include <optional>
#include <stdexcept>
#include <utility>

export module vrock.utils:Task;

namespace vrock::utils
{
    export template <typename ReturnType = void>
    class Task;

    export struct PromiseBase
    {
        friend struct FinalAwaitable;
        struct FinalAwaitable
        {
            [[nodiscard]] auto await_ready( ) const noexcept -> bool
            {
                return false;
            }

            template <typename PromiseType>
            auto await_suspend( std::coroutine_handle<PromiseType> coroutine ) noexcept -> std::coroutine_handle<>
            {
                auto &promise = coroutine.promise( );
                if ( promise._continuation != nullptr )
                    return promise._continuation;
                else
                    return std::noop_coroutine( );
            }

            auto await_resume( ) noexcept -> void
            {
                // no-op
            }
        };

        PromiseBase( ) noexcept = default;
        ~PromiseBase( ) = default;

        auto initial_suspend( ) noexcept
        {
            return std::suspend_always{ };
        }

        auto final_suspend( ) noexcept
        {
            return FinalAwaitable{ };
        }

        auto unhandled_exception( ) -> void
        {
            _exception_ptr = std::current_exception( );
        }

        auto continuation( std::coroutine_handle<> continuation ) noexcept -> void
        {
            _continuation = continuation;
        }

    protected:
        std::coroutine_handle<> _continuation{ nullptr };
        std::exception_ptr _exception_ptr{ };
    };

    export template <typename ReturnType>
    struct Promise final : public PromiseBase
    {
        using task_type = Task<ReturnType>;
        using coroutine_handle = std::coroutine_handle<Promise<ReturnType>>;
        static constexpr bool return_type_is_reference = std::is_reference_v<ReturnType>;

        Promise( ) noexcept = default;
        ~Promise( ) = default;

        auto get_return_object( ) noexcept -> task_type;

        auto return_value( ReturnType value ) -> void
        {
            if constexpr ( return_type_is_reference )
                _return_value = std::addressof( value );
            else
                _return_value = { std::move( value ) };
        }

        auto result( ) const & -> decltype( auto )
        {
            if ( _exception_ptr )
                std::rethrow_exception( _exception_ptr );

            if constexpr ( return_type_is_reference )
            {
                if ( _return_value )
                    return static_cast<ReturnType>( *_return_value );
            }
            else
            {
                if ( _return_value.has_value( ) )
                    return _return_value.value( );
            }
            throw std::runtime_error{ "return value was never set." };
        }

        auto result( ) && -> decltype( auto )
        {
            if ( _exception_ptr )
                std::rethrow_exception( _exception_ptr );

            if constexpr ( return_type_is_reference )
            {
                if ( _return_value )
                    return static_cast<ReturnType>( *_return_value );
            }
            else
            {
                if ( _return_value.has_value( ) )
                    return std::move( _return_value.value( ) );
            }

            throw std::runtime_error{ "The return value was never set, did you execute the coroutine?" };
        }

        Promise &operator=( const Promise & ) = delete;
        Promise &operator=( Promise &&other ) = delete;

    private:
        using held_type = std::conditional_t<return_type_is_reference, std::remove_reference_t<ReturnType> *,
                                             std::optional<ReturnType>>;
        held_type _return_value{ };
    };

    export template <>
    struct Promise<void> final : public PromiseBase
    {
        using task_type = Task<void>;
        using coroutine_handle = std::coroutine_handle<Promise<void>>;

        Promise( ) noexcept = default;
        ~Promise( ) = default;

        inline auto get_return_object( ) noexcept -> task_type;

        auto return_void( ) noexcept -> void
        {
        }

        auto result( ) -> void
        {
            if ( _exception_ptr )
                std::rethrow_exception( _exception_ptr );
        }
    };

    template <typename ReturnType>
    class [[nodiscard]] Task
    {
    public:
        using task_type = Task<ReturnType>;
        using promise_type = Promise<ReturnType>;
        using coroutine_handle = std::coroutine_handle<promise_type>;

        struct AwaitableBase
        {
            AwaitableBase( coroutine_handle coroutine ) noexcept : _coroutine( coroutine )
            {
            }

            auto await_ready( ) const noexcept -> bool
            {
                return !_coroutine || _coroutine.done( );
            }

            auto await_suspend( std::coroutine_handle<> awaiting_coroutine ) noexcept -> std::coroutine_handle<>
            {
                _coroutine.promise( ).continuation( awaiting_coroutine );
                return _coroutine;
            }

            std::coroutine_handle<promise_type> _coroutine{ nullptr };
        };

        Task( ) noexcept : _coroutine( nullptr )
        {
        }

        explicit Task( coroutine_handle handle ) : _coroutine( handle )
        {
        }
        Task( const Task & ) = delete;
        Task( Task &&other ) noexcept : _coroutine( std::exchange( other._coroutine, nullptr ) )
        {
        }

        ~Task( )
        {
            if ( _coroutine != nullptr )
                _coroutine.destroy( );
        }

        [[nodiscard]] auto is_ready( ) const noexcept -> bool
        {
            return _coroutine == nullptr || _coroutine.done( );
        }

        auto resume( ) -> bool
        {
            if ( !_coroutine.done( ) )
                _coroutine.resume( );
            return !_coroutine.done( );
        }

        auto destroy( ) -> bool
        {
            if ( _coroutine != nullptr )
            {
                _coroutine.destroy( );
                _coroutine = nullptr;
                return true;
            }
            return false;
        }

        auto promise( ) & -> promise_type &
        {
            return _coroutine.promise( );
        }

        auto promise( ) const & -> const promise_type &
        {
            return _coroutine.promise( );
        }

        auto promise( ) && -> promise_type &&
        {
            return std::move( _coroutine.promise( ) );
        }

        auto handle( ) -> coroutine_handle
        {
            return _coroutine;
        }

        auto operator=( const Task & ) -> Task & = delete;

        auto operator=( Task &&other ) noexcept -> Task &
        {
            if ( std::addressof( other ) != this )
            {
                if ( _coroutine != nullptr )
                    _coroutine.destroy( );
                _coroutine = std::exchange( other._coroutine, nullptr );
            }
            return *this;
        }

        auto operator co_await( ) const & noexcept
        {
            struct Awaitable : public AwaitableBase
            {
                Awaitable( coroutine_handle handle ) : AwaitableBase( handle )
                {
                }

                auto await_resume( ) -> decltype( auto )
                {
                    return this->_coroutine.promise( ).result( );
                }
            };
            return Awaitable{ _coroutine };
        }

        auto operator co_await( ) const && noexcept
        {
            struct Awaitable : public AwaitableBase
            {
                Awaitable( coroutine_handle handle ) : AwaitableBase( handle )
                {
                }

                auto await_resume( ) -> decltype( auto )
                {
                    return std::move( this->_coroutine.promise( ) ).result( );
                }
            };
            return Awaitable{ _coroutine };
        }

    private:
        coroutine_handle _coroutine{ nullptr };
    };

    template <typename ReturnType>
    inline auto Promise<ReturnType>::get_return_object( ) noexcept -> Task<ReturnType>
    {
        return Task<ReturnType>{ coroutine_handle::from_promise( *this ) };
    }

    inline auto Promise<void>::get_return_object( ) noexcept -> Task<>
    {
        return Task<>{ coroutine_handle::from_promise( *this ) };
    }
} // namespace vrock::utils
