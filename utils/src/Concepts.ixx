module;

#include <coroutine>
#include <utility>

export module vrock.utils:Concepts;

namespace vrock::utils
{
    template <typename type, typename... types>
    concept in_types = ( std::same_as<type, types> || ... );

    template <typename type>
    concept Awaiter = requires( type t, std::coroutine_handle<> c ) {
        {
            t.await_ready( )
        } -> std::same_as<bool>;
        {
            t.await_suspend( c )
        } -> in_types<void, bool, std::coroutine_handle<>>;
        {
            t.await_resume( )
        };
    };

    template <typename Type>
    concept member_co_await_awaitable = requires( Type t ) {
        {
            t.operator co_await( )
        } -> Awaiter;
    };

    template <typename Type>
    concept global_co_await_awaitable = requires( Type t ) {
        {
            operator co_await( t )
        } -> Awaiter;
    };

    template <typename Type>
    concept Awaitable = member_co_await_awaitable<Type> || global_co_await_awaitable<Type> || Awaiter<Type>;

    template <typename Type>
    concept AwaiterVoid = Awaiter<Type> && requires( Type t ) {
        {
            t.await_resume( )
        } -> std::same_as<void>;
    };

    template <typename Type>
    concept member_co_await_awaitable_void = requires( Type t ) {
        {
            t.operator co_await( )
        } -> AwaiterVoid;
    };

    template <typename Type>
    concept global_co_await_awaitable_void = requires( Type t ) {
        {
            operator co_await( t )
        } -> AwaiterVoid;
    };

    template <typename Type>
    concept AwaitableVoid =
        member_co_await_awaitable_void<Type> || global_co_await_awaitable_void<Type> || AwaiterVoid<Type>;

    template <Awaitable Awaitable, typename = void>
    struct awaitable_traits
    {
    };

    export template <Awaitable Awaitable>
    auto get_awaiter( Awaitable &&value )
    {
        if constexpr ( member_co_await_awaitable<Awaitable> )
            return std::forward<Awaitable>( value ).operator co_await( );
        else if constexpr ( global_co_await_awaitable<Awaitable> )
            return operator co_await( std::forward<Awaitable>( value ) );
        else if constexpr ( Awaiter<Awaitable> )
            return std::forward<Awaitable>( value );
    }

    template <Awaitable Awaitable>
    struct awaitable_traits<Awaitable>
    {
        using awaiter_type = decltype( get_awaiter( std::declval<Awaitable>( ) ) );
        using awaiter_return_type = decltype( std::declval<awaiter_type>( ).await_resume( ) );
    };
} // namespace vrock::utils