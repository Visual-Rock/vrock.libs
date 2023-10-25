module;

#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <mutex>
#include <type_traits>
#include <utility>

export module vrock.utils;

export import :ByteArray;
export import :FutureHelper;
export import :Timer;
export import :List;
export import :Task;
export import :Concepts;

namespace vrock::utils
{
    class AwaitEvent
    {
    public:
        explicit AwaitEvent( bool initially_set = false ) : _set( initially_set )
        {
        }

        AwaitEvent( const AwaitEvent & ) = delete;
        AwaitEvent( AwaitEvent && ) = delete;
        auto operator=( const AwaitEvent & ) -> AwaitEvent & = delete;
        auto operator=( AwaitEvent && ) -> AwaitEvent & = delete;
        ~AwaitEvent( ) = default;

        auto set( ) noexcept -> void
        {
            {
                std::lock_guard<std::mutex> g{ _mutex };
                _set = true;
            }

            _cv.notify_all( );
        }

        auto reset( ) noexcept -> void
        {
            std::lock_guard<std::mutex> g{ _mutex };
            _set = false;
        }

        auto wait( ) noexcept -> void
        {
            std::unique_lock<std::mutex> lk{ _mutex };
            _cv.wait( lk, [ this ] { return _set; } );
        }

    private:
        std::mutex _mutex;
        std::condition_variable _cv;
        bool _set{ false };
    };

    class AwaitTaskPromiseBase
    {
    public:
        AwaitTaskPromiseBase( ) noexcept = default;

        auto initial_suspend( ) noexcept -> std::suspend_always
        {
            return { };
        }

        auto unhandled_exception( ) -> void
        {
            _exception = std::current_exception( );
        }

    protected:
        AwaitEvent *_event{ nullptr };
        std::exception_ptr _exception;

        ~AwaitTaskPromiseBase( ) = default;
    };

    template <typename ReturnType>
    class AwaitTaskPromise : public AwaitTaskPromiseBase
    {
    public:
        using coroutine_type = std::coroutine_handle<AwaitTaskPromise<ReturnType>>;

        AwaitTaskPromise( ) noexcept = default;
        ~AwaitTaskPromise( ) = default;

        auto start( AwaitEvent &event )
        {
            _event = &event;
            coroutine_type::from_promise( *this ).resume( );
        }

        auto get_return_object( ) noexcept
        {
            return coroutine_type::from_promise( *this );
        }

        auto yield_value( ReturnType &&value ) noexcept
        {
            _return_value = std::addressof( value );
            return final_suspend( );
        }

        auto final_suspend( ) noexcept
        {
            struct completion_notifier
            {
                auto await_ready( ) const noexcept
                {
                    return false;
                }
                auto await_suspend( coroutine_type coroutine ) const noexcept
                {
                    coroutine.promise( )._event->set( );
                }
                auto await_resume( ) noexcept { };
            };

            return completion_notifier{ };
        }

        auto result( ) -> ReturnType &&
        {
            if ( _exception )
                std::rethrow_exception( _exception );
            return static_cast<ReturnType &&>( *_return_value );
        }
        void return_void( ) noexcept
        {
        }

    private:
        std::remove_reference_t<ReturnType> *_return_value;
    };

    template <>
    class AwaitTaskPromise<void> : public AwaitTaskPromiseBase
    {
        using coroutine_type = std::coroutine_handle<AwaitTaskPromise<void>>;

    public:
        AwaitTaskPromise( ) noexcept = default;
        ~AwaitTaskPromise( ) = default;

        auto start( AwaitEvent &event )
        {
            _event = &event;
            coroutine_type::from_promise( *this ).resume( );
        }

        auto get_return_object( ) noexcept
        {
            return coroutine_type::from_promise( *this );
        }

        auto final_suspend( ) noexcept
        {
            struct completion_notifier
            {
                auto await_ready( ) const noexcept
                {
                    return false;
                }
                auto await_suspend( coroutine_type coroutine ) const noexcept
                {
                    coroutine.promise( )._event->set( );
                }
                auto await_resume( ) noexcept { };
            };

            return completion_notifier{ };
        }

        auto return_void( ) noexcept -> void
        {
        }

        auto result( ) -> void
        {
            if ( _exception )
                std::rethrow_exception( _exception );
        }
    };

    template <typename ReturnType>
    class AwaitTask
    {
    public:
        using promise_type = AwaitTaskPromise<ReturnType>;
        using coroutine_type = std::coroutine_handle<promise_type>;

        explicit AwaitTask( coroutine_type coroutine ) noexcept : _coroutine( coroutine )
        {
        }

        AwaitTask( const AwaitTask & ) = delete;
        AwaitTask( AwaitTask &&other ) noexcept : _coroutine( std::exchange( other.m_coroutine, coroutine_type{ } ) )
        {
        }

        ~AwaitTask( )
        {
            if ( _coroutine )
                _coroutine.destroy( );
        }

        auto start( AwaitEvent &event ) noexcept
        {
            _coroutine.promise( ).start( event );
        }

        auto return_value( ) -> decltype( auto )
        {
            if constexpr ( std::is_same_v<void, ReturnType> )
            {
                // Propagate exceptions.
                _coroutine.promise( ).result( );
                return;
            }
            else
            {
                return _coroutine.promise( ).result( );
            }
        }

        auto operator=( const AwaitTask & ) -> AwaitTask & = delete;
        auto operator=( AwaitTask &&other ) noexcept -> AwaitTask &
        {
            if ( std::addressof( other ) != this )
                _coroutine = std::exchange( other.m_coroutine, coroutine_type{ } );
            return *this;
        }

    private:
        coroutine_type _coroutine;
    };

    template <awaitable awaitable_type, typename return_type = awaitable_traits<awaitable_type>::awaiter_return_type>
    auto make_await_task( awaitable_type &&a ) -> AwaitTask<return_type>;

    template <awaitable awaitable_type, typename ReturnType>
    auto make_await_task( awaitable_type &&a ) -> AwaitTask<ReturnType>
    {
        if constexpr ( std::is_void_v<ReturnType> )
        {
            co_await std::forward<awaitable_type>( a );
            co_return;
        }
        else
        {
            co_yield co_await std::forward<awaitable_type>( a );
        }
    }

    export template <awaitable awaitable_type>
    auto await( awaitable_type &&a ) -> decltype( auto )
    {
        AwaitEvent e{ };
        auto task = make_await_task( std::forward<awaitable_type>( a ) );
        task.start( e );
        e.wait( );

        return task.return_value( );
    }
} // namespace vrock::utils