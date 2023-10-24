module;

#include <coroutine>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <utility>

export module vrock.utils.Task;

namespace vrock::utils
{
    export template <typename ReturnType = void>
    class Task;

    struct PromiseBase
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

        auto continuation( std::coroutine_handle<> continuation ) noexcept -> void
        {
            _continuation = continuation;
        }

    protected:
        std::coroutine_handle<> _continuation{ nullptr };
    };

    template <typename ReturnType = void>
    struct Promise final : public PromiseBase
    {
        enum class TaskState : std::uint8_t
        {
            Empty,
            Value,
            Error
        };

        using task_type = Task<ReturnType>;
        using coroutine_handle = std::coroutine_handle<Promise<ReturnType>>;

        static constexpr bool return_type_is_reference = std::is_reference_v<ReturnType>;
        using stored_type = std::conditional_t<return_type_is_reference, std::remove_reference_t<ReturnType> *,
                                               std::remove_const_t<ReturnType>>;
        static constexpr auto storage_align = alignof( std::exception_ptr ) > alignof( stored_type )
                                                  ? alignof( std::exception_ptr )
                                                  : alignof( stored_type );

        static constexpr auto storage_size = sizeof( std::exception_ptr ) > sizeof( stored_type )
                                                 ? sizeof( std::exception_ptr )
                                                 : sizeof( stored_type );

        Promise( ) noexcept : _state( TaskState::Empty )
        {
        }
        Promise( const Promise & ) = delete;
        Promise( Promise &&other ) = delete;
        ~Promise( )
        {
            if ( _state == TaskState::Value )
            {
                if constexpr ( not return_type_is_reference )
                {
                    access_value( ).~stored_type( );
                }
            }
            else if ( _state == TaskState::Error )
            {
                access_exception( ).~exception_ptr( );
            }
        }

        auto get_return_object( ) noexcept -> task_type;

        template <typename Value>
            requires( return_type_is_reference and std::is_constructible_v<ReturnType, Value &&> ) or
                    ( not return_type_is_reference and std::is_constructible_v<stored_type, Value &&> )
        auto return_value( Value &&value ) -> void
        {
            if constexpr ( return_type_is_reference )
            {
                auto ref = static_cast<Value &&>( value );
                new ( _storage ) stored_type( std::addressof( ref ) );
            }
            else
                new ( _storage ) stored_type( static_cast<Value &&>( value ) );
            _state = TaskState::Value;
        }

        auto return_value( stored_type value ) -> void
            requires( not return_type_is_reference )
        {
            if constexpr ( std::is_move_constructible_v<stored_type> )
                new ( _storage ) stored_type( std::move( value ) );
            else
                new ( _storage ) stored_type( value );
            _state = TaskState::Value;
        }

        auto unhandled_exception( ) noexcept -> void
        {
            new ( _storage ) std::exception_ptr( std::current_exception( ) );
            _state = TaskState::Error;
        }

        auto result( ) & -> decltype( auto )
        {
            switch ( _state )
            {
            case TaskState::Value:
                if constexpr ( return_type_is_reference )
                    return static_cast<ReturnType>( *access_value( ) );
                else
                    return static_cast<const stored_type &>( access_value( ) );
            case TaskState::Error:
                std::rethrow_exception( access_exception( ) );
            default:
                throw std::runtime_error{ "return value was never set." };
            }
        }

        auto result( ) const & -> decltype( auto )
        {
            switch ( _state )
            {
            case TaskState::Value:
                if constexpr ( return_type_is_reference )
                    return static_cast<std::add_const_t<ReturnType>>( *access_value( ) );
                else
                    return static_cast<const stored_type &>( access_value( ) );
            case TaskState::Error:
                std::rethrow_exception( access_exception( ) );
            default:
                throw std::runtime_error{ "return value was never set." };
            }
        }

        auto result( ) && -> decltype( auto )
        {
            switch ( _state )
            {
            case TaskState::Value:
                if constexpr ( return_type_is_reference )
                    return static_cast<ReturnType>( *access_value( ) );
                else if constexpr ( std::is_move_constructible_v<stored_type> )
                    return static_cast<stored_type &&>( access_value( ) );
                else
                    return static_cast<const stored_type &&>( access_value( ) );
            case TaskState::Error:
                std::rethrow_exception( access_exception( ) );
            default:
                throw std::runtime_error{ "return value was never set." };
            }
        }

        Promise &operator=( const Promise & ) = delete;
        Promise &operator=( Promise &&other ) = delete;

    private:
        alignas( storage_align ) unsigned char _storage[ storage_size ];
        TaskState _state;

        auto access_value( ) const noexcept -> const stored_type &
        {
            return *std::launder( reinterpret_cast<const stored_type *>( _storage ) );
        }

        auto access_value( ) noexcept -> stored_type &
        {
            return *std::launder( reinterpret_cast<stored_type *>( _storage ) );
        }

        [[nodiscard]] auto access_exception( ) const noexcept -> const std::exception_ptr &
        {
            return *std::launder( reinterpret_cast<const std::exception_ptr *>( _storage ) );
        }

        auto access_exception( ) noexcept -> std::exception_ptr &
        {
            return *std::launder( reinterpret_cast<std::exception_ptr *>( _storage ) );
        }
    };

    template <>
    struct Promise<void> final : public PromiseBase
    {
        using task_type = Task<void>;
        using coroutine_handle = std::coroutine_handle<Promise<void>>;

        Promise( ) noexcept = default;
        Promise( const Promise & ) = delete;
        Promise( Promise &&other ) = delete;
        Promise &operator=( const Promise & ) = delete;
        Promise &operator=( Promise &&other ) = delete;
        ~Promise( ) = default;

        inline auto get_return_object( ) noexcept -> Task<>;

        auto return_void( ) noexcept -> void
        {
        }

        auto unhandled_exception( ) noexcept -> void
        {
            _exception_ptr = std::current_exception( );
        }

        auto result( ) -> void
        {
            if ( _exception_ptr )
                std::rethrow_exception( _exception_ptr );
        }

    private:
        std::exception_ptr _exception_ptr{ nullptr };
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
        Task( Task &&other ) noexcept : _coroutine( std::exchange( other.m_coroutine, nullptr ) )
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
                _coroutine = std::exchange( other.m_coroutine, nullptr );
            }
            return *this;
        }

        auto operator co_await( ) const & noexcept
        {
            struct awaitable : public AwaitableBase
            {
                auto await_resume( ) -> decltype( auto )
                {
                    return this->_coroutine.promise( ).result( );
                }
            };
            return awaitable{ _coroutine };
        }

        auto operator co_await( ) const && noexcept
        {
            struct awaitable : public AwaitableBase
            {
                auto await_resume( ) -> decltype( auto )
                {
                    return std::move( this->_coroutine.promise( ) ).result( );
                }
            };
            return awaitable{ _coroutine };
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
