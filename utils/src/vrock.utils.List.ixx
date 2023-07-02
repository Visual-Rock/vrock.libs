module;

export module vrock.utils.List;

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <algorithm>

#include <cmath>
#include <exception>
#include <functional>
#include <map>
#include <tuple>

namespace vrock::utils
{

    /*! \cond */
    template <typename R> constexpr auto less( const R &a, const R &b ) -> bool
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

    /// List that adds Linq functionality
    ///
    /// this class adds Linq features like aggregate, select and group_by functions.
    template <class T>
    export class List : public std::vector<T>
    {
    public:
        /// Empty list
        List( )
        {
        }
        /// @param v List Elements
        List( std::vector<T> v ) : std::vector<T>( v )
                {
                }

        ~List( )
        {
        }

    public:
        /// @return size of the list
        auto count( ) -> size_t
        {
            return this->size( );
        }
        /// applies an expression on the list
        /// @param exp an expression that is used for the for loop
        auto for_each( std::function<void( T )> exp ) -> void
        {
            std::for_each( this->begin( ), this->end( ), exp );
        }

        /// applies an aggregate function on the List
        /// @param exp aggregate function to use. it takes the element and the value of the runs befor. returns the new
        /// value
        /// @return result after the aggregate function was run on every element
        template <class R> auto aggregate( std::function<R( T, R )> exp ) -> R
        {
            R ret = R( );
            for_each( [ & ]( T i ) { ret = exp( i, ret ); } );
            return ret;
        }
        /// sets the given list with the value of the current list
        /// @param l list to copy to
        /// @return the current list
        auto let( List<T> *l ) -> List<T>
    {
        l->resize( this->size( ) );
        std::copy( this->begin( ), this->end( ), l->begin( ) );
        return *this;
    }
    /// checks if a given value is contained in the List
    /// @param val value to check
    /// @return value in this List
    auto contains( T val ) -> bool
    {
        return any( [ val ]( T it ) { return it == val; } );
    }
    /// checks if the List contains any element
    auto any( ) -> bool
    {
        return !this->empty( );
    }
    /// checks if the List contains any specific element
    /// @param exp expression to check for the wanted element
    auto any( std::function<bool( T )> exp ) -> bool
    {
        return std::find_if( this->begin( ), this->end( ), exp ) != this->end( );
    }
    /// checks if all elements in the List conform to an expression
    /// @param exp expresion to check against
    auto all( std::function<bool( T )> exp ) -> bool
    {
        if ( std::find_if_not( this->begin( ), this->end( ), exp ) == this->end( ) )
            return true;
        return false;
    }
    /// check if the given List and the current List are equal
    auto sequence_equal( List<T> o ) -> bool
    {
        return *this == o;
    }
    /// concatenates two lists together and returns the result
    auto concat( List<T> o ) -> List<T>
    {
        this->insert( this->end( ), o.begin( ), o.end( ) );
        return *this;
    }
    /// skips elements in the list and return the remaining
    /// @param a amount to skip
    auto skip( size_t a ) -> List<T>
    {
        if ( a > this->size( ) )
            return List<T>( );
        return List<T>( std::vector<T>( this->begin( ) + a, this->end( ) ) );
    }
    /// skip elements till the expression evaluates to false
    auto skip_while( std::function<bool( T )> exp ) -> List<T>
    {
        auto it = std::find_if_not( this->begin( ), this->end( ), exp );
        if ( it == this->end( ) )
            return List<T>( );
        return List<T>( std::vector<T>( it, this->end( ) ) );
    }
    /// @param a amount of elements to take
    /// @return List with the first a elements from this List
    auto take( size_t a ) -> List<T>
    {
        if ( a > this->size( ) )
            return List<T>( std::vector<T>( this->begin( ), this->end( ) ) );
        return List<T>( std::vector<T>( this->begin( ), this->begin( ) + a ) );
    }
    /// takes elements till the expression evaluates to false
    auto take_while( std::function<bool( T )> exp ) -> List<T>
    {
        auto it = std::find_if_not( this->begin( ), this->end( ), exp );
        if ( it == this->end( ) )
            return List<T>( std::vector<T>( this->begin( ), this->end( ) ) );
        return List<T>( std::vector<T>( this->begin( ), it ) );
    }
    /// revers the order of elements
    auto revers( ) -> List<T>
    {
        std::reverse( this->begin( ), this->end( ) );
        return *this;
    }
    /// Get the first element conforming to an expression. if no element conforms throw an exception
    /// @param exp expression to check against
    /// @return first element that complies with the expression
    auto first( std::function<bool( T )> exp ) -> T
    {
        auto res = std::find_if( this->begin( ), this->end( ), exp );
        if ( res == this->end( ) )
            throw std::runtime_error( "No element matching predicate found" );
        return *res;
    }
    /// Get the first element conforming to an expression. if no element conforms return a default value
    /// @param exp expression to check against
    /// @return first element that complies with the expression
    auto first_or_default( std::function<bool( T )> exp, T _default = T( ) ) -> T
    {
        auto res = std::find_if( this->begin( ), this->end( ), exp );
        if ( res == this->end( ) )
            return _default;
        return *res;
    }
    /// Get the last element conforming to an expression. if no element conforms throw an exception
    /// @param exp expression to check against
    /// @return last element that complies with the expression
    auto last( std::function<bool( T )> exp ) -> T
    {
        auto res = std::find_if( this->rbegin( ), this->rend( ), exp );
        if ( res == this->rend( ) )
            throw std::runtime_error( "No element matching predicate found" );
        return *res;
    }
    /// Get the last element conforming to an expression. if no element conforms return a default value
    /// @param exp expression to check against
    /// @return last element that complies with the expression
    auto last_or_default( std::function<bool( T )> exp, T _default = T( ) ) -> T
    {
        auto res = std::find_if( this->rbegin( ), this->rend( ), exp );
        if ( res == this->rend( ) )
            return _default;
        return *res;
    }
    /// get a single element conforming to an expression. if multiple conform it throws an exception
    /// @param exp expression to check against
    /// @return the element conforming to exp
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
    /// get a single element conforming to an expression. if multiple conform it throws an exception and if no
    /// element was found return the default
    /// @param exp expression to check against
    /// @return the element conforming to exp
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
    /// @brief selects all elements and applies the given expression
    /// @tparam R type of the newly created list
    /// @param exp expression which takes an element of type T and return an element of type R
    /// @return List with all elements
    template <class R> auto select( std::function<R( T )> exp ) -> List<R>
    {
        List<R> ret = List<R>( );
        std::for_each( this->begin( ), this->end( ), [ & ]( T i ) { ret.push_back( exp( i ) ); } );
        return ret;
    }
    /// @brief apply a filter (exp) to the list
    /// @param exp expression to decide if an element should be in the resulting list
    /// @return list with all elements complying to the expression
    auto where( std::function<bool( T )> exp ) -> List<T>
    {
        auto ret = List<T>( );
        std::for_each( this->begin( ), this->end( ), [ & ]( T i ) {
            if ( exp( i ) )
                ret.push_back( i );
        } );
        return ret;
    }
    /// @brief Joins the current list on o
    /// @tparam R type of the List o
    /// @tparam E type of the resulting List
    /// @param o List to join on
    /// @param exp1 expression to that returns if two elements should be joined
    /// @param exp2 expression that returns a new joined element
    /// @return resulting joined List
    template <class R, class E>
    auto join( List<R> o, std::function<bool( T, R )> exp1, std::function<E( T, R )> exp2 ) -> List<E>
    {
        auto ret = List<E>( );
        std::for_each( this->begin( ), this->end( ), [ & ]( auto i ) {
            std::for_each( o.begin( ), o.end( ), [ & ]( auto j ) {
                if ( exp1( i, j ) )
                    ret.push_back( exp2( i, j ) );
            } );
        } );
        return ret;
    }

    /// @brief groups the list by the given parameters
    /// @tparam ...R types of the parameters to group by
    /// @param ...params the parameter to group by
    /// @return resulting grouped map
    template <typename... R> auto group_by( R... params )
    {
        using key_type = std::tuple<typename remove_pointers<R>::type...>;

        auto comp = make_tuple_less<R...>( );
        auto counter = std::map<key_type, std::vector<int>, decltype( comp )>( comp );

        for ( size_t i = 0; i < this->size( ); i++ )
        {
            const auto key = std::make_tuple( ( this->at( i ).*params )... );
            counter[ key ].emplace_back( i );
        }

        auto res = std::map<key_type, List<T>, decltype( comp )>( comp );

        for ( auto &[ key, i ] : counter )
        {
            auto &g = res[ key ];
            g = List<T>( std::vector<T>( i.size( ) ) );
            std::transform( i.begin( ), i.end( ), g.begin( ), [ & ]( auto index ) { return this->at( index ); } );
        }
        return res;
    }
    /// @brief orders the List by a given predicate
    /// @param exp expression to order by
    /// @return ordered List
    auto order_by( std::function<bool( T, T )> exp ) -> List<T>
    {
        std::sort( this->begin( ), this->end( ), exp );
        return *this;
    }
    /// @return List containing distinct elements
    auto distinct( ) -> List<T>
    {
        auto ret = List<T>( );
        std::for_each( this->begin( ), this->end( ), [ & ]( T i ) {
            if ( !ret.contains( i ) )
                ret.push_back( i );
        } );
        return ret;
    }
    /// @brief applies a union on the current and the given list
    /// @param other List to perform the union on
    /// @return resulting union list
    auto union_list( List<T> other ) -> List<T>
    {
        return this->concat( other ).distinct( );
    }
    /// @brief returns all elements contained in both Lists
    /// @param other List to intersect with
    /// @return resulting intersection
    auto intersect( List<T> other ) -> List<T>
    {
        auto ret = List<T>( );
        this->for_each( [ & ]( T i ) {
            if ( other.contains( i ) )
                ret.push_back( i );
        } );
        return ret;
    }
    /// @brief returns a List filled with all entries that are not in the given list
    /// @param other list of elements to exclude
    /// @return filtered list
    auto except( List<T> other ) -> List<T>
    {
        auto ret = List<T>( );
        this->for_each( [ & ]( T i ) {
            if ( !other.contains( i ) )
                ret.push_back( i );
        } );
        return ret;
    }

    /// @brief converts the current List to a std::vector
    /// @return list as vector
    auto to_vector( ) -> std::vector<T>
    {
        return std::vector<T>( this->begin( ), this->end( ) );
    }
    /// @brief converts the current List to a std::deque
    /// @return list as deque
    auto to_deque( ) -> std::deque<T>
    {
        return std::deque<T>( this->begin( ), this->end( ) );
    }
    /// @brief converts the current List to a std::forward_list
    /// @return list as forward_list
    auto to_forward_list( ) -> std::forward_list<T>
    {
        return std::forward_list<T>( this->begin( ), this->end( ) );
    }
    /// @brief converts the current List to a std::list
    /// @return list as std::list
    auto to_list( ) -> std::list<T>
    {
        return std::list<T>( this->begin( ), this->end( ) );
    }

    private:
    /*! \cond */
    template <class M> struct remove_pointers
    {
        typedef M type;
    };

    template <typename M, typename N> struct remove_pointers<N M::*>
    {
        typedef N type;
    };

    template <size_t i, size_t size, typename... R> struct tuple_less_t
    {
        constexpr static auto tuple_less( const std::tuple<R...> &a, const std::tuple<R...> &b ) -> bool
        {
            return less( std::get<i>( a ), std::get<i>( b ) ) ||
                   ( !less( std::get<i>( b ), std::get<i>( a ) ) &&
                     tuple_less_t<i + 1, size, R...>::tuple_less( a, b ) );
        }
    };

    template <size_t size, typename... Elements> struct tuple_less_t<size, size, Elements...>
    {
        constexpr static auto tuple_less( const std::tuple<Elements...> &a, const std::tuple<Elements...> &b )
        -> bool
        {
            return false;
        }
    };

    template <typename... R> constexpr auto make_tuple_less( )
    {
        constexpr auto s = sizeof...( R );
        return tuple_less_t<0u, s, typename remove_pointers<R>::type...>::tuple_less;
    }
    /*! \endcond */
    };

    /// @brief flattens the given list
    /// @tparam T type of the List Lists
    /// @param list collection of lists to flatten
    /// @return flattened List
    template <class T> auto select_many( List<List<T>> const &list ) -> List<T>
    {
    List<T> flattened;
    for ( auto const &l : list )
    flattened.insert( flattened.end( ), l.begin( ), l.end( ) );
    return flattened;
    }

    /// @brief flattens the given list
    /// @tparam T type of the List Lists
    /// @param list collection of lists to flatten
    /// @return flattened List
    template <class T>
    auto select_many( List<std::vector<T>> const &list ) -> List<T>
    {
        List<T> flattened;
        for ( auto const &v : list )
        flattened.insert( flattened.end( ), v.begin( ), v.end( ) );
        return flattened;
    }
} // namespace vrock::utils