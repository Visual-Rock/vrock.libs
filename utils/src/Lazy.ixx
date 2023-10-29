module;

#include <functional>

export module vrock.utils:Lazy;

namespace vrock::utils
{
    export template <typename T>
    class Lazy
    {
    public:
        Lazy( ) = delete;
        explicit Lazy( std::function<T( )> fn ) : _fn( std::move( fn ) )
        {
        }

        Lazy( const Lazy<T> &lazy ) : _loaded( lazy._loaded )
        {
            if ( lazy._loaded )
                _value = lazy._value;
            else
                _fn = lazy._fn;
        }

        Lazy( Lazy<T> &&lazy ) noexcept : _loaded( lazy._loaded )
        {
            if ( lazy._loaded )
                _value = std::move( lazy._value );
            else
                _fn = std::move( lazy._fn );
        }

        T &operator( )( )
        {
            if ( !_loaded )
            {
                _value = _fn( );
                _loaded = true;
            }
            return _value;
        }

        Lazy<T> &operator=( const Lazy<T> &lazy )
        {
            if ( lazy._loaded )
                _value = lazy._value;
            else
                _fn = lazy._fn;
            _loaded = lazy._loaded;
            return *this;
        }

        Lazy<T> &operator=( Lazy<T> &&lazy ) noexcept
        {
            if ( lazy._loaded )
                _value = std::move( lazy._value );
            else
                _fn = std::move( lazy._fn );
            _loaded = lazy._loaded;
            return *this;
        }

    private:
        T _value;
        std::function<T( )> _fn;
        bool _loaded = false;
    };
} // namespace vrock::utils
