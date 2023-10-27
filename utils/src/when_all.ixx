module;

#include <atomic>
#include <coroutine>
#include <mutex>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

export module vrock.utils:when_all;

import :Concepts;

namespace vrock::utils
{
    class WhenAllLatch
    {
    public:
        WhenAllLatch( std::size_t count ) noexcept : _count( count + 1 )
        {
        }

        WhenAllLatch( const WhenAllLatch & ) = delete;
        WhenAllLatch( WhenAllLatch &&other )
            : _count( other._count.load( std::memory_order::acquire ) ),
              _awaiting_coroutine( std::exchange( other._awaiting_coroutine, nullptr ) )
        {
        }

        [[nodiscard]] auto is_ready( ) const noexcept -> bool
        {
            return _awaiting_coroutine != nullptr && _awaiting_coroutine.done( );
        }

        auto try_await( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
        {
            _awaiting_coroutine = awaiting_coroutine;
            return _count.fetch_sub( 1, std::memory_order::acq_rel ) > 1;
        }

        auto notify_awaitable_completed( ) noexcept -> void
        {
            if ( _count.fetch_sub( 1, std::memory_order::acq_rel ) == 1 )
            {
                _awaiting_coroutine.resume( );
            }
        }

        auto operator=( const WhenAllLatch & ) -> WhenAllLatch & = delete;
        auto operator=( WhenAllLatch &&other ) noexcept -> WhenAllLatch &
        {
            if ( std::addressof( other ) != this )
            {
                _count.store( other._count.load( std::memory_order::acquire ), std::memory_order::relaxed );
                _awaiting_coroutine = std::exchange( other._awaiting_coroutine, nullptr );
            }
            return *this;
        }

    private:
        std::atomic<std::size_t> _count;
        std::coroutine_handle<> _awaiting_coroutine{ nullptr };
    };

    template <typename TaskContainerType>
    class WhenAllReadyAwaitable;

    template <typename ReturnType>
    class WhenAllTask;

    template <>
    class WhenAllReadyAwaitable<std::tuple<>>
    {
    public:
        constexpr WhenAllReadyAwaitable( ) noexcept = default;

        explicit constexpr WhenAllReadyAwaitable( std::tuple<> ) noexcept
        {
        }

        [[nodiscard]] constexpr auto await_ready( ) const noexcept -> bool
        {
            return true;
        }
        auto await_suspend( std::coroutine_handle<> ) noexcept -> void
        {
        }
        auto await_resume( ) const noexcept -> std::tuple<>
        {
            return { };
        }
    };

    template <typename... TaskTypes>
    class WhenAllReadyAwaitable<std::tuple<TaskTypes...>>
    {
    public:
        explicit WhenAllReadyAwaitable( TaskTypes &&...tasks ) noexcept(
            std::conjunction<std::is_nothrow_move_constructible<TaskTypes>...>::value )
            : _latch( sizeof...( TaskTypes ) ), _tasks( std::move( tasks )... )
        {
        }

        explicit WhenAllReadyAwaitable( std::tuple<TaskTypes...> &&tasks ) noexcept(
            std::is_nothrow_move_constructible_v<std::tuple<TaskTypes...>> )
            : _latch( sizeof...( TaskTypes ) ), _tasks( std::move( tasks ) )
        {
        }

        WhenAllReadyAwaitable( const WhenAllReadyAwaitable & ) = delete;
        WhenAllReadyAwaitable( WhenAllReadyAwaitable &&other )
            : _latch( std::move( other._latch ) ), _tasks( std::move( other._tasks ) )
        {
        }

        auto operator co_await( ) & noexcept
        {
            struct Awaiter
            {
                explicit Awaiter( WhenAllReadyAwaitable &awaitable ) noexcept : m_awaitable( awaitable )
                {
                }

                auto await_ready( ) const noexcept -> bool
                {
                    return m_awaitable.is_ready( );
                }

                auto await_suspend( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
                {
                    return m_awaitable.try_await( awaiting_coroutine );
                }

                auto await_resume( ) noexcept -> std::tuple<TaskTypes...> &
                {
                    return m_awaitable._tasks;
                }

            private:
                WhenAllReadyAwaitable &m_awaitable;
            };

            return Awaiter{ *this };
        }

        auto operator co_await( ) && noexcept
        {
            struct Awaiter
            {
                explicit Awaiter( WhenAllReadyAwaitable &awaitable ) noexcept : _awaitable( awaitable )
                {
                }

                auto await_ready( ) const noexcept -> bool
                {
                    return _awaitable.is_ready( );
                }

                auto await_suspend( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
                {
                    return _awaitable.try_await( awaiting_coroutine );
                }

                auto await_resume( ) noexcept -> std::tuple<TaskTypes...> &&
                {
                    return std::move( _awaitable._tasks );
                }

                auto operator=( const WhenAllReadyAwaitable & ) -> WhenAllReadyAwaitable & = delete;
                auto operator=( WhenAllReadyAwaitable && ) -> WhenAllReadyAwaitable & = delete;

            private:
                WhenAllReadyAwaitable &_awaitable;
            };

            return Awaiter{ *this };
        }

    private:
        auto is_ready( ) const noexcept -> bool
        {
            return _latch.is_ready( );
        }

        auto try_await( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
        {
            std::apply( [ this ]( auto &&...tasks ) { ( ( tasks.start( _latch ) ), ... ); }, _tasks );
            return _latch.try_await( awaiting_coroutine );
        }

        WhenAllLatch _latch;
        std::tuple<TaskTypes...> _tasks;
    };

    template <typename TaskContainerType>
    class WhenAllReadyAwaitable
    {
    public:
        explicit WhenAllReadyAwaitable( TaskContainerType &&tasks ) noexcept
            : _latch( std::size( tasks ) ), _tasks( std::forward<TaskContainerType>( tasks ) )
        {
        }

        WhenAllReadyAwaitable( const WhenAllReadyAwaitable & ) = delete;
        WhenAllReadyAwaitable( WhenAllReadyAwaitable &&other ) noexcept(
            std::is_nothrow_move_constructible_v<TaskContainerType> )
            : _latch( std::move( other._latch ) ), _tasks( std::move( _tasks ) )
        {
        }

        auto operator co_await( ) & noexcept
        {
            struct Awaiter
            {
                Awaiter( WhenAllReadyAwaitable &awaitable ) : m_awaitable( awaitable )
                {
                }

                auto await_ready( ) const noexcept -> bool
                {
                    return m_awaitable.is_ready( );
                }

                auto await_suspend( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
                {
                    return m_awaitable.try_await( awaiting_coroutine );
                }

                auto await_resume( ) noexcept -> TaskContainerType &
                {
                    return m_awaitable._tasks;
                }

            private:
                WhenAllReadyAwaitable &m_awaitable;
            };

            return Awaiter{ *this };
        }

        auto operator co_await( ) && noexcept
        {
            struct Awaiter
            {
                Awaiter( WhenAllReadyAwaitable &awaitable ) : m_awaitable( awaitable )
                {
                }

                auto await_ready( ) const noexcept -> bool
                {
                    return m_awaitable.is_ready( );
                }

                auto await_suspend( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
                {
                    return m_awaitable.try_await( awaiting_coroutine );
                }

                auto await_resume( ) noexcept -> TaskContainerType &&
                {
                    return std::move( m_awaitable._tasks );
                }

                auto operator=( const WhenAllReadyAwaitable & ) -> WhenAllReadyAwaitable & = delete;
                auto operator=( WhenAllReadyAwaitable & ) -> WhenAllReadyAwaitable & = delete;

            private:
                WhenAllReadyAwaitable &m_awaitable;
            };

            return Awaiter{ *this };
        }

    private:
        [[nodiscard]] auto is_ready( ) const noexcept -> bool
        {
            return _latch.is_ready( );
        }

        auto try_await( std::coroutine_handle<> awaiting_coroutine ) noexcept -> bool
        {
            for ( auto &task : _tasks )
                task.start( _latch );
            return _latch.try_await( awaiting_coroutine );
        }

        WhenAllLatch _latch;
        TaskContainerType _tasks;
    };

    template <typename ReturnType>
    class WhenAllTaskPromise
    {
    public:
        using coroutine_handle_type = std::coroutine_handle<WhenAllTaskPromise<ReturnType>>;

        WhenAllTaskPromise( ) noexcept
        {
        }

        auto get_return_object( ) noexcept
        {
            return coroutine_handle_type::from_promise( *this );
        }

        auto initial_suspend( ) noexcept -> std::suspend_always
        {
            return { };
        }

        auto final_suspend( ) noexcept
        {
            struct CompletionNotifier
            {
                auto await_ready( ) const noexcept -> bool
                {
                    return false;
                }
                auto await_suspend( coroutine_handle_type coroutine ) const noexcept -> void
                {
                    coroutine.promise( )._latch->notify_awaitable_completed( );
                }
                auto await_resume( ) const noexcept
                {
                }
            };

            return CompletionNotifier{ };
        }

        auto unhandled_exception( ) noexcept
        {
            _exception_ptr = std::current_exception( );
        }

        auto yield_value( ReturnType &&value ) noexcept
        {
            _return_value = std::addressof( value );
            return final_suspend( );
        }

        auto start( WhenAllLatch &latch ) noexcept -> void
        {
            _latch = &latch;
            coroutine_handle_type::from_promise( *this ).resume( );
        }

        auto return_value( ) & -> ReturnType &
        {
            if ( _exception_ptr )
                std::rethrow_exception( _exception_ptr );
            return *_return_value;
        }

        auto return_value( ) && -> ReturnType &&
        {
            if ( _exception_ptr )
                std::rethrow_exception( _exception_ptr );
            return std::forward( *_return_value );
        }

    private:
        WhenAllLatch *_latch{ nullptr };
        std::exception_ptr _exception_ptr;
        std::add_pointer_t<ReturnType> _return_value;
    };

    template <>
    class WhenAllTaskPromise<void>
    {
    public:
        using coroutine_handle_type = std::coroutine_handle<WhenAllTaskPromise<void>>;

        WhenAllTaskPromise( ) noexcept = default;

        auto get_return_object( ) noexcept
        {
            return coroutine_handle_type::from_promise( *this );
        }

        auto initial_suspend( ) noexcept -> std::suspend_always
        {
            return { };
        }

        auto final_suspend( ) noexcept
        {
            struct CompletionNotifier
            {
                auto await_ready( ) const noexcept -> bool
                {
                    return false;
                }

                auto await_suspend( coroutine_handle_type coroutine ) const noexcept -> void
                {
                    coroutine.promise( )._latch->notify_awaitable_completed( );
                }

                auto await_resume( ) const noexcept -> void
                {
                }
            };

            return CompletionNotifier{ };
        }

        auto unhandled_exception( ) noexcept -> void
        {
            _exception_ptr = std::current_exception( );
        }

        auto return_void( ) noexcept -> void
        {
        }

        auto result( ) -> void
        {
            if ( _exception_ptr )
            {
                std::rethrow_exception( _exception_ptr );
            }
        }

        auto start( WhenAllLatch &latch ) -> void
        {
            _latch = &latch;
            coroutine_handle_type::from_promise( *this ).resume( );
        }

    private:
        WhenAllLatch *_latch{ nullptr };
        std::exception_ptr _exception_ptr;
    };

    struct Void
    {
    };

    template <typename ReturnType>
    class WhenAllTask
    {
    public:
        template <typename TaskContainerType>
        friend class WhenAllReadyAwaitable;

        using promise_type = WhenAllTaskPromise<ReturnType>;
        using coroutine_handle_type = typename promise_type::coroutine_handle_type;

        WhenAllTask( coroutine_handle_type coroutine ) noexcept : _coroutine( coroutine )
        {
        }

        WhenAllTask( const WhenAllTask & ) = delete;
        WhenAllTask( WhenAllTask &&other ) noexcept
            : _coroutine( std::exchange( other._coroutine, coroutine_handle_type{ } ) )
        {
        }

        WhenAllTask( )
        {
            if ( _coroutine != nullptr )
                _coroutine.destroy( );
        }

        auto return_value( ) & -> decltype( auto )
        {
            if constexpr ( std::is_void_v<ReturnType> )
            {
                _coroutine.promise( ).result( );
                return Void{ };
            }
            else
            {
                return _coroutine.promise( ).return_value( );
            }
        }

        auto return_value( ) const & -> decltype( auto )
        {
            if constexpr ( std::is_void_v<ReturnType> )
            {
                _coroutine.promise( ).result( );
                return Void{ };
            }
            else
                return _coroutine.promise( ).return_value( );
        }

        auto return_value( ) && -> decltype( auto )
        {
            if constexpr ( std::is_void_v<ReturnType> )
            {
                _coroutine.promise( ).result( );
                return Void{ };
            }
            else
                return _coroutine.promise( ).return_value( );
        }

        auto operator=( const WhenAllTask & ) -> WhenAllTask & = delete;
        auto operator=( WhenAllTask && ) -> WhenAllTask & = delete;

    private:
        auto start( WhenAllLatch &latch ) noexcept -> void
        {
            _coroutine.promise( ).start( latch );
        }

        coroutine_handle_type _coroutine;
    };
    
    template <Awaitable Awaitable, typename ReturnType>
    static auto make_when_all_task( Awaitable a ) -> WhenAllTask<ReturnType>
    {
        if constexpr ( std::is_void_v<ReturnType> )
        {
            co_await static_cast<Awaitable &&>( a );
            co_return;
        }
        else
            co_yield co_await static_cast<Awaitable &&>( a );
    }

    export template <Awaitable... AwaitablesType>
    [[nodiscard]] auto when_all( AwaitablesType... awaitables )
    {
        return WhenAllReadyAwaitable<
            std::tuple<WhenAllTask<typename awaitable_traits<AwaitablesType>::awaiter_return_type>...>>(
            std::make_tuple( make_when_all_task( std::move( awaitables ) )... ) );
    }

    /*
    template <std::ranges::range RangeType, Awaitable AwaitableType = std::ranges::range_value_t<RangeType>,
              typename ReturnType = typename awaitable_traits<AwaitableType>::awaiter_return_type>
    [[nodiscard]] auto when_all( RangeType awaitables ) -> WhenAllReadyAwaitable<std::vector<WhenAllTask<ReturnType>>>
    {
        std::vector<WhenAllTask<ReturnType>> output_tasks;

        if constexpr ( std::ranges::sized_range<RangeType> )
            output_tasks.reserve( std::size( awaitables ) );

        for ( auto &a : awaitables )
            output_tasks.emplace_back( make_when_all_task( std::move( a ) ) );

        return WhenAllReadyAwaitable( std::move( output_tasks ) );
    }*/
} // namespace vrock::utils