#include <coroutine>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

import vrock.utils;
using namespace vrock::utils;

Generator<int> fib( )
{
    auto a = 0, b = 1;
    while ( true )
        co_yield std::exchange( a, std::exchange( b, a + b ) );
}

Task<int> double_and_add( int a, int b )
{
    co_return ( a * 2 ) + b;
}

Task<void> do_stuff( )
{
    std::cout << "Hello World" << std::endl;
    co_return;
}

int main( )
{
    {
        std::vector<Task<int>> tasks{ };
        tasks.reserve( 2 );
        tasks.emplace_back( double_and_add( 2, 3 ) );
        tasks.emplace_back( double_and_add( 5, 6 ) );

        auto res = await( when_all( std::move( tasks ) ) );
        for ( const auto &task : res )
        {
            std::cout << task.value( ) << " ";
        }
        std::cout << std::endl;
    }

    {
        auto res = await( when_all( double_and_add( 2, 3 ), double_and_add( 5, 6 ), do_stuff( ) ) );
        std::cout << std::get<0>( res ).value( ) << " " << std::get<1>( res ).value( ) << std::endl;

        static_assert( std::is_same_v<decltype( std::get<2>( res ).value( ) ), Void> );
    }

    {
        auto range = fib( ) | std::views::drop( 6 ) | std::views::take( 3 );
        for ( auto num : range )
        {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
}