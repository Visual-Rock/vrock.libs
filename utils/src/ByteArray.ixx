module;

#include <cinttypes>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

/**
 * @brief module that holds the ByteArray class
 */
export module vrock.utils.ByteArray;

namespace vrock::utils
{
    /**
     * @class ByteArray
     *
     * `ByteArray` is a class designed to handle memory operations on byte arrays.
     * It offers capabilities such as creating a byte array from size or string,
     * resizing, creating sub-arrays, appending byte arrays, and several ways to return the byte array data.
     * It does not incorporate any concurrency control mechanisms,  * making it essential to implement
     * external synchronization when being accessed or modified by multiple threads.
     */
    export template <typename Alloc = std::allocator<std::uint8_t>>
    class ByteArray
    {
    private:
        ByteArray( std::size_t len, std::uint8_t *data ) : size_( len ), data_( data )
        {
        }

    public:
        /**
         * Default constructor, creates an empty ByteArray.
         */
        ByteArray( ) : size_( 0 ), data_( nullptr )
        {
        }

        /**
         * Constructor, creates a ByteArray of a given length.
         * @param len Length of new ByteArray.
         */
        explicit ByteArray( std::size_t len ) : size_( len ), data_( allocate_ptr( len ) )
        {
            std::memset( data_, 0, size_ );
        }

        /**
         * Constructor, creates a ByteArray by copying data from std::string.
         * @param data Input std::string.
         */
        explicit ByteArray( std::string data ) : size_( data.size( ) ), data_( allocate_ptr( data.size( ) ) )
        {
            std::memcpy( data_, data.data( ), size_ );
        }

        ByteArray( const ByteArray &arr )
        {
            size_ = arr.size( );
            data_ = allocate_ptr( arr.size( ) );
            std::memcpy( data_, arr.data( ), size_ );
        }

        ~ByteArray( )
        {
            if ( data_ )
                deallocate_ptr( size_, data_ );
        }

        /**
         * Returns size of ByteArray.
         * @return Size of ByteArray.
         */
        [[nodiscard]] auto size( ) const noexcept -> std::size_t
        {
            return size_;
        }

        /**
         * Returns pointer to ByteArray data.
         * @return Pointer to ByteArray data.
         */
        [[nodiscard]] auto data( ) const noexcept -> std::uint8_t *
        {
            return data_;
        }

        /**
         * Returns ByteArray data as a std::string.
         * @return ByteArray data as a std::string.
         */
        [[nodiscard]] auto to_string( ) const noexcept -> std::string
        {
            if ( !data_ )
                return "";
            return { (char *)data_, size_ };
        }

        /**
         * Returns ByteArray data as a hexadecimal std::string.
         * @return ByteArray data as a hexadecimal std::string.
         */
        [[nodiscard]] auto to_hex_string( ) const noexcept -> std::string
        {
            if ( size_ == 0 )
                return "";
            std::stringstream str;
            for ( size_t i = 0; i < size_; i++ )
                str << std::setw( 2 ) << std::setfill( '0' ) << std::hex << (int)data_[ i ];
            return str.str( );
        }

        /**
         * Resizes ByteArray, copying up to new_len bytes from the old data.
         * @param new_len New length of ByteArray.
         */
        auto reserve( std::size_t new_len ) noexcept -> void
        {
            std::uint8_t *n = allocate_ptr( new_len );
            std::memcpy( n, data_, std::min( new_len, size_ ) );
            deallocate_ptr( size_, data_ );
            data_ = n;
            size_ = new_len;
        }

        /**
         * Returns a sub array of ByteArray starting from start and up to len characters.
         * @param start Start index of sub array.
         * @param len Length of sub array.
         * @return Sub array of ByteArray.
         */
        [[nodiscard]] auto subarr( std::size_t start, std::size_t len = 18446744073709551615UL ) -> ByteArray
        {
            if ( len == 18446744073709551615UL )
                len = size_ - start;
            if ( start + len > size_ )
                throw std::out_of_range( "failed to create sub array! byte array not long enough." );
            std::uint8_t *arr = allocate_ptr( len );
            std::memcpy( arr, data_ + start, len );
            return { len, arr };
        }

        /**
         * Appends other ByteArray to this ByteArray.
         * @param arr ByteArray to append.
         */
        auto append( const ByteArray &arr ) noexcept -> void
        {
            std::uint8_t *n = allocate_ptr( size_ + arr.size_ );
            std::memcpy( n, data_, size_ );
            std::memcpy( n + size_, arr.data_, arr.size_ );
            deallocate_ptr( size_, data_ );
            data_ = n;
            size_ = size_ + arr.size_;
        }

        /**
         * Return the byte at given position.
         * @param pos Index of byte to return
         * @return Byte at given position.
         */
        [[nodiscard]] auto at( std::size_t pos ) const -> std::uint8_t &
        {
            if ( pos > size_ - 1 )
                throw std::out_of_range( "out of bounds" );
            return data_[ pos ];
        }

        /**
         * Operator overloading for direct const data access.
         * @param i Index of byte to return.
         * @return Byte at position i.
         */
        const uint8_t &operator[]( std::size_t i ) const
        {
            return at( i );
        }

        uint8_t &operator[]( std::size_t i )
        {
            return at( i );
        }

    private:
        std::size_t size_;
        std::uint8_t *data_;
        Alloc alloc_;

        /**
         * Allocates a new uint8_t array of given length.
         * @param len Length of the new array.
         * @return Pointer to newly allocated array.
         */
        [[nodiscard]] auto allocate_ptr( std::size_t len ) -> std::uint8_t *
        {
            return alloc_.allocate( len );
        }

        auto deallocate_ptr( std::size_t len, std::uint8_t *ptr ) -> void
        {
            alloc_.deallocate( ptr, len );
        }

    public:
        bool operator==( const ByteArray &other ) const
        {
            if ( size_ != other.size_ )
                return false;
            for ( std::size_t i = 0; i < size_; i++ )
                if ( other[ i ] != this->at( i ) )
                    return false;
            return true;
        }
    };

    export auto from_hex_string( const std::string &str ) -> ByteArray<>
    {
        std::string s = str + ( ( str.length( ) % 2 == 1 ) ? "0" : "" ); // Append zero if needed
        auto data = ByteArray<>( s.length( ) / 2 );

        for ( size_t i = 0; i < data.size( ); ++i )
            data[ i ] = std::stoul( s.substr( i * 2, 2 ), nullptr, 16 );

        return data;
    }
} // namespace vrock::utils