module;

#include <cinttypes>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>

/**
 * @brief module that holds the ByteArray class
 */
export module vrock.utils.ByteArray;

export namespace vrock::utils
{
    /**
     * @class ByteArray
     *
     * `ByteArray` is a simple class to handle memory operations on byte arrays.
     *
     * Internally, it uses a `std::uint8_t*` to manage the byte data and a `std::size_t` for size of byte array.
     * The class provides basic functionality such as:
     *
     * - Constructing byte array from size or `std::string`
     * - Getting size of byte array
     * - Getting a pointer to byte data
     * - Getting byte array as a string
     * - Getting byte array as a hexadecimal string
     * - Resizing byte array
     * - Creating sub array from byte array
     * - Appending one byte array to another
     * - Accessing individual bytes in byte array
     *
     * For dynamic memory management, `std::malloc`, `std::free` and `std::memcpy` are used.
     * This class does not provide any mechanism for concurrency control.
     * If multiple threads are accessing and modifying byte array,
     * appropriate synchronization mechanism needs to be employed externally.
     */
    class ByteArray
    {
    private:
        ByteArray(std::size_t len, std::uint8_t *data) : size_(len), data_(data) {}

    public:
        /**
         * Default constructor, creates an empty ByteArray.
         */
        ByteArray() : size_(0), data_(nullptr) {}

        /**
         * Constructor, creates a ByteArray of a given length.
         * @param len Length of new ByteArray.
         */
        ByteArray(std::size_t len) : size_(len), data_(allocate_ptr(len)) {}

        /**
         * Constructor, creates a ByteArray by copying data from std::string.
         * @param data Input std::string.
         */
        explicit ByteArray(std::string data) : size_(data.size()), data_(allocate_ptr(data.size()))
        {
            std::memcpy(data_, data.data(), size_);
        }

        /**
         * Default destructor, frees data memory if any.
         */
        ~ByteArray()
        {
            if (data_)
                std::free(data_);
        }

        /**
         * Returns size of ByteArray.
         * @return Size of ByteArray.
         */
        auto size() const noexcept -> std::size_t { return size_; }

        /**
         * Returns pointer to ByteArray data.
         * @return Pointer to ByteArray data.
         */
        auto data() const noexcept -> std::uint8_t * { return data_; }

        /**
         * Returns ByteArray data as a std::string.
         * @return ByteArray data as a std::string.
         */
        auto to_string() const noexcept -> std::string
        {
            if (!data_)
                return "";
            return {(char *)data_, size_};
        }

        /**
         * Returns ByteArray data as a hexadecimal std::string.
         * @return ByteArray data as a hexadecimal std::string.
         */
        auto to_hex_string() const noexcept -> std::string
        {
            if (size_ == 0)
                return "";
            std::stringstream str;
            for (size_t i = 0; i < size_; i++)
                str << std::setw(2) << std::setfill('0') << std::hex << (int)data_[i];
            return str.str();
        }

        /**
         * Resizes ByteArray, copying up to new_len bytes from the old data.
         * @param new_len New length of ByteArray.
         */
        auto reserve(std::size_t new_len) noexcept -> void
        {
            std::uint8_t *n = allocate_ptr(new_len);
            std::memcpy(n, data_, std::min(new_len, size_));
            std::free(data_);
            data_ = n;
            size_ = new_len;
        }

        /**
         * Returns a sub array of ByteArray starting from start and up to len characters.
         * @param start Start index of sub array.
         * @param len Length of sub array.
         * @return Sub array of ByteArray.
         */
        auto subarr(std::size_t start, std::size_t len = 18446744073709551615UL) const -> ByteArray
        {
            if (len == 18446744073709551615UL)
                len = size_ - start;
            if (start + len > size_)
                throw std::out_of_range("failed to create sub array! byte array not long enough.");
            std::uint8_t *arr = allocate_ptr(len);
            std::memcpy(arr, data_ + start, len);
            return {len, arr};
        }

        /**
         * Appends other ByteArray to this ByteArray.
         * @param arr ByteArray to append.
         */
        auto append(const ByteArray &arr) noexcept -> void
        {
            std::uint8_t *n = allocate_ptr(size_ + arr.size_);
            std::memcpy(n, data_, size_);
            std::memcpy(n + size_, arr.data_, arr.size_);
            std::free(data_);
            data_ = n;
            size_ = size_ + arr.size_;
        }

        /**
         * Return the byte at given position.
         * @param pos Index of byte to return
         * @return Byte at given position.
         */
        auto at(std::size_t pos) const -> std::uint8_t &
        {
            if (pos > size_ - 1)
                throw std::out_of_range("out of bounds");
            return data_[pos];
        }

        /**
         * Operator overloading for direct data access.
         * @param i Index of byte to return.
         * @return Byte at position i.
         */
        uint8_t &operator[](std::size_t i)
        {
            return at(i);
        }
        /**
         * Operator overloading for direct const data access.
         * @param i Index of byte to return.
         * @return Byte at position i.
         */
        const uint8_t &operator[](std::size_t i) const
        {
            return at(i);
        }

    private:
        std::size_t size_;
        std::uint8_t *data_;

        /**
         * Allocates a new uint8_t array of given length.
         * @param len Length of the new array.
         * @return Pointer to newly allocated array.
         */
        auto allocate_ptr(std::size_t len) const -> std::uint8_t *
        {
            return (std::uint8_t *)std::malloc(sizeof(std::uint8_t) * len);
        }
    };
} // namespace vrock::utils
