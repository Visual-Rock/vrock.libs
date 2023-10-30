#pragma once

#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include <cmath>
#include <exception>
#include <functional>
#include <map>
#include <ostream>
#include <stdexcept>
#include <tuple>

namespace vrock::utils
{

    /*! \cond */
    template <typename R>
    constexpr auto less( const R &a, const R &b ) -> bool
    {
        if constexpr ( std::is_floating_point<R>::value )
        {
            if ( std::isnan( b ) )
            {
                if ( std::isnan( a ) )
                {
                    return false;
                }
                return true;
            }
        }
        return a < b;
    }
    /*! \endcond */

    template <class T, typename Alloc = std::allocator<T>>
    class List : public std::vector<T, Alloc>
    {
    public:
        auto count( ) -> size_t
        {
            return this->size( );
        }

        auto for_each( std::function<void( T )> exp ) -> void
        {
            std::for_each( this->begin( ), this->end( ), exp );
        }

        template <class R>
        auto aggregate( std::function<R( T, R )> exp, R start = R( ) ) -> R
        {
            R ret = start;
            for_each( [ & ]( T i ) { ret = exp( i, ret ); } );
            return ret;
        }

        auto let( List<T> &l ) -> List<T, Alloc>
        {
            l.resize( this->size( ) );
            std::copy( this->begin( ), this->end( ), l.begin( ) );
            return *this;
        }

        auto contains( T val ) -> bool
        {
            return any( [ val ]( T it ) { return it == val; } );
        }

        auto any( ) -> bool
        {
            return !this->empty( );
        }

        auto any( std::function<bool( T )> exp ) -> bool
        {
            return std::find_if( this->begin( ), this->end( ), exp ) != this->end( );
        }

        auto all( std::function<bool( T )> exp ) -> bool
        {
            if ( std::find_if_not( this->begin( ), this->end( ), exp ) == this->end( ) )
                return true;
            return false;
        }

        auto sequence_equal( List<T> o ) -> bool
        {
            return *this == o;
        }

        auto concat( List<T> o ) -> List<T, Alloc>
        {
            this->insert( this->end( ), o.begin( ), o.end( ) );
            return *this;
        }

        auto skip( size_t a ) -> List<T, Alloc>
        {
            if ( a > this->size( ) )
                return List<T>( );
            return List<T, Alloc>( this->begin( ) + a, this->end( ) );
        }

        auto skip_while( std::function<bool( T )> exp ) -> List<T, Alloc>
        {
            auto it = std::find_if_not( this->begin( ), this->end( ), exp );
            if ( it == this->end( ) )
                return List<T, Alloc>( );
            return List<T, Alloc>( it, this->end( ) );
        }

        auto take( size_t a ) -> List<T, Alloc>
        {
            if ( a > this->size( ) )
                return List<T, Alloc>( this->begin( ), this->end( ) );
            return List<T, Alloc>( this->begin( ), this->begin( ) + a );
        }

        auto take_while( std::function<bool( T )> exp ) -> List<T, Alloc>
        {
            auto it = std::find_if_not( this->begin( ), this->end( ), exp );
            if ( it == this->end( ) )
                return List<T, Alloc>( this->begin( ), this->end( ) );
            return List<T, Alloc>( this->begin( ), it );
        }

        auto revers( ) -> List<T>
        {
            std::reverse( this->begin( ), this->end( ) );
            return *this;
        }

        auto first( std::function<bool( T )> exp ) -> T
        {
            auto res = std::find_if( this->begin( ), this->end( ), exp );
            if ( res == this->end( ) )
                throw std::runtime_error( "No element matching predicate found" );
            return *res;
        }

        auto first_or_default( std::function<bool( T )> exp, T _default = T( ) ) -> T
        {
            auto res = std::find_if( this->begin( ), this->end( ), exp );
            if ( res == this->end( ) )
                return _default;
            return *res;
        }

        auto last( std::function<bool( T )> exp ) -> T
        {
            auto res = std::find_if( this->rbegin( ), this->rend( ), exp );
            if ( res == this->rend( ) )
                throw std::runtime_error( "No element matching predicate found" );
            return *res;
        }

        auto last_or_default( std::function<bool( T )> exp, T _default = T( ) ) -> T
        {
            auto res = std::find_if( this->rbegin( ), this->rend( ), exp );
            if ( res == this->rend( ) )
                return _default;
            return *res;
        }

        auto single( std::function<bool( T )> exp ) -> T
        {
            auto res1 = std::find_if( this->begin( ), this->end( ), exp );
            if ( res1 == this->end( ) )
                throw std::runtime_error( "No element matching predicate found" );
            auto res2 = std::find_if( res1 + 1, this->end( ), exp );
            if ( res2 != this->end( ) )
                throw std::runtime_error( "more than one element matching the predicate found" );
            return *res1;
        }

        auto single_or_default( std::function<bool( T )> exp, T _default = T( ) ) -> T
        {
            auto res1 = std::find_if( this->begin( ), this->end( ), exp );
            if ( res1 == this->end( ) )
                return _default;
            auto res2 = std::find_if( res1 + 1, this->end( ), exp );
            if ( res2 != this->end( ) )
                throw std::runtime_error( "more than one element matching the predicate found" );
            return *res1;
        }

        template <class R>
        auto select( std::function<R( T )> exp ) -> List<R, Alloc>
        {
            List<R, Alloc> ret = List<R, Alloc>( );
            std::for_each( this->begin( ), this->end( ), [ & ]( T i ) { ret.emplace_back( exp( i ) ); } );
            return ret;
        }

        auto where( std::function<bool( T )> exp ) -> List<T, Alloc>
        {
            auto ret = List<T, Alloc>( );
            std::copy_if( this->begin( ), this->end( ), std::inserter( ret.begin( ) ), exp );
            return ret;
        }

        template <class R, class E>
        auto join( List<R> o, std::function<bool( T, R )> exp1, std::function<E( T, R )> exp2 ) -> List<E>
        {
            auto ret = List<E, Alloc>( );
            std::for_each( this->begin( ), this->end( ), [ & ]( auto i ) {
                std::for_each( o.begin( ), o.end( ), [ & ]( auto j ) {
                    if ( exp1( i, j ) )
                        ret.push_back( exp2( i, j ) );
                } );
            } );
            return ret;
        }

        template <typename... R>
        auto group_by( R... params )
        {
            using key_type = std::tuple<typename remove_pointers<R>::type...>;

            auto comp = make_tuple_less<R...>( );
            auto counter = std::map<key_type, std::vector<int>, decltype( comp )>( comp );

            for ( size_t i = 0; i < this->size( ); i++ )
            {
                const auto key = std::make_tuple( ( this->at( i ).*params )... );
                counter[ key ].emplace_back( i );
            }

            auto res = std::map<key_type, List<T, Alloc>, decltype( comp )>( comp );

            for ( auto &[ key, i ] : counter )
            {
                auto &g = res[ key ];
                g = List<T, Alloc>( std::vector<T>( i.size( ) ) );
                std::transform( i.begin( ), i.end( ), g.begin( ), [ & ]( auto index ) { return this->at( index ); } );
            }
            return res;
        }

        auto order_by( std::function<bool( T, T )> exp ) -> List<T>
        {
            std::sort( this->begin( ), this->end( ), exp );
            return *this;
        }

        auto distinct( ) -> List<T, Alloc>
        {
            auto ret = List<T, Alloc>( );
            std::for_each( this->begin( ), this->end( ), [ & ]( T i ) {
                if ( !ret.contains( i ) )
                    ret.push_back( i );
            } );
            return ret;
        }

        auto union_list( List<T> other ) -> List<T, Alloc>
        {
            auto ret = List<T, Alloc>( );
            std::set_union( this->begin( ), this->end( ), other.begin( ), other.end( ),
                            std::inserter( ret, ret.end( ) ) );
            return ret;
        }

        auto intersect( List<T> other ) -> List<T, Alloc>
        {
            auto ret = List<T, Alloc>( );
            std::set_intersection( this->begin( ), this->end( ), other.begin( ), other.end( ),
                                   std::inserter( ret, ret.end( ) ) );
            return ret;
        }

        auto except( List<T> other ) -> List<T, Alloc>
        {
            auto ret = List<T, Alloc>( );
            std::set_difference( this->begin( ), this->end( ), other.begin( ), other.end( ),
                                 std::inserter( ret, ret.end( ) ) );
            return ret;
        }

        auto to_vector( ) -> std::vector<T, Alloc>
        {
            return std::vector<T, Alloc>( this->begin( ), this->end( ) );
        }

        auto to_deque( ) -> std::deque<T>
        {
            return std::deque<T, Alloc>( this->begin( ), this->end( ) );
        }

        auto to_forward_list( ) -> std::forward_list<T>
        {
            return std::forward_list<T, Alloc>( this->begin( ), this->end( ) );
        }

        auto to_list( ) -> std::list<T>
        {
            return std::list<T, Alloc>( this->begin( ), this->end( ) );
        }

        friend std::ostream &operator<<( std::ostream &os, const List &list )
        {
            for ( int i = 0; i < list.size( ) - 1; ++i )
                os << list[ i ] << ", ";
            os << list[ list.size( ) - 1 ];
            return os;
        }

    private:
        /*! \cond */
        template <class M>
        struct remove_pointers
        {
            typedef M type;
        };

        template <typename M, typename N>
        struct remove_pointers<N M::*>
        {
            typedef N type;
        };

        template <size_t i, size_t size, typename... R>
        struct tuple_less_t
        {
            constexpr static auto tuple_less( const std::tuple<R...> &a, const std::tuple<R...> &b ) -> bool
            {
                return less( std::get<i>( a ), std::get<i>( b ) ) ||
                       ( !less( std::get<i>( b ), std::get<i>( a ) ) &&
                         tuple_less_t<i + 1, size, R...>::tuple_less( a, b ) );
            }
        };

        template <size_t size, typename... Elements>
        struct tuple_less_t<size, size, Elements...>
        {
            constexpr static auto tuple_less( const std::tuple<Elements...> &a, const std::tuple<Elements...> &b )
                -> bool
            {
                return false;
            }
        };

        template <typename... R>
        constexpr auto make_tuple_less( )
        {
            constexpr auto s = sizeof...( R );
            return tuple_less_t<0u, s, typename remove_pointers<R>::type...>::tuple_less;
        }
        /*! \endcond */
    };

    template <class T>
    auto select_many( List<List<T>> &list ) -> List<T>
    {
        List<T> flattened;
        for ( auto const &l : list )
            flattened.insert( flattened.end( ), l.begin( ), l.end( ) );
        return flattened;
    }

    template <class T>
    auto select_many( List<std::vector<T>> &list ) -> List<T>
    {
        List<T> flattened;
        for ( auto const &v : list )
            flattened.insert( flattened.end( ), v.begin( ), v.end( ) );
        return flattened;
    }
} // namespace vrock::utils