#include "vrock/security.hpp"

#include <cryptopp/sha.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/md5.h"

namespace vrock::security
{
    auto md5( byte_span_t data ) -> return_t
    {
        CryptoPP::Weak::MD5 hash;
        return_t return_{ };
        return_.reserve( hash.DigestSize( ) );
        hash.Update( data.data( ), hash.DigestSize( ) );
        hash.Final( return_.data( ) );
        return return_;
    }

    auto sha224( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA224 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha256( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA256 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha384( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA384 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha512( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA512 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto md5( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::Weak::MD5 hash;
        auto hashed = std::make_shared<utils::ByteArray<>>( hash.DigestSize( ) );
        hash.Update( data->data( ), data->size( ) );
        hash.Final( hashed->data( ) );
        return hashed;
    }

    auto sha224( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::SHA224 hash;
        auto hashed = std::make_shared<utils::ByteArray<>>( hash.DigestSize( ) );
        hash.Update( data->data( ), data->size( ) );
        hash.Final( hashed->data( ) );
        return hashed;
    }

    auto sha256( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::SHA256 hash;
        auto hashed = std::make_shared<utils::ByteArray<>>( hash.DigestSize( ) );
        hash.Update( data->data( ), data->size( ) );
        hash.Final( hashed->data( ) );
        return hashed;
    }

    auto sha384( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::SHA384 hash;
        auto hashed = std::make_shared<utils::ByteArray<>>( hash.DigestSize( ) );
        hash.Update( data->data( ), data->size( ) );
        hash.Final( hashed->data( ) );
        return hashed;
    }

    auto sha512( const std::shared_ptr<utils::ByteArray<>> &data ) -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::SHA512 hash;
        auto hashed = std::make_shared<utils::ByteArray<>>( hash.DigestSize( ) );
        hash.Update( data->data( ), data->size( ) );
        hash.Final( hashed->data( ) );
        return hashed;
    }
} // namespace vrock::security