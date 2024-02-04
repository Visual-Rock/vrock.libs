#include "vrock/security.hpp"

#include <cryptopp/sha.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/md5.h"

namespace vrock::security
{
    auto md5( byte_span_t data ) -> return_t
    {
        CryptoPP::Weak::MD5 hash;
        return_t hashed{ };
        hashed.resize( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha224( byte_span_t data ) -> return_t
    {
        CryptoPP::SHA224 hash;
        return_t hashed{ };
        hashed.resize( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha256( byte_span_t data ) -> return_t
    {
        CryptoPP::SHA256 hash;
        return_t hashed{ };
        hashed.resize( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha384( byte_span_t data ) -> return_t
    {
        CryptoPP::SHA384 hash;
        return_t hashed{ };
        hashed.resize( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto sha512( byte_span_t data ) -> return_t
    {
        CryptoPP::SHA512 hash;
        return_t hashed{ };
        hashed.resize( hash.DigestSize( ) );
        hash.Update( data.data( ), data.size( ) );
        hash.Final( hashed.data( ) );
        return hashed;
    }

    auto md5( string_view_t data ) -> return_string_t
    {
        CryptoPP::Weak::MD5 hash;
        std::string hashed;
        hashed.resize( hash.DigestSize( ) );
        hash.Update( (CryptoPP::byte *)data.data( ), data.size( ) );
        hash.Final( reinterpret_cast<CryptoPP::byte *>( hashed.data( ) ) );
        return hashed;
    }

    auto sha224( string_view_t data ) -> return_string_t
    {
        CryptoPP::SHA224 hash;
        std::string hashed;
        hashed.resize( hash.DigestSize( ) );
        hash.Update( (CryptoPP::byte *)data.data( ), data.size( ) );
        hash.Final( reinterpret_cast<CryptoPP::byte *>( hashed.data( ) ) );
        return hashed;
    }

    auto sha256( string_view_t data ) -> return_string_t
    {
        CryptoPP::SHA256 hash;
        std::string hashed;
        hashed.resize( hash.DigestSize( ) );
        hash.Update( (CryptoPP::byte *)data.data( ), data.size( ) );
        hash.Final( reinterpret_cast<CryptoPP::byte *>( hashed.data( ) ) );
        return hashed;
    }

    auto sha384( string_view_t data ) -> return_string_t
    {
        CryptoPP::SHA384 hash;
        std::string hashed;
        hashed.resize( hash.DigestSize( ) );
        hash.Update( (CryptoPP::byte *)data.data( ), data.size( ) );
        hash.Final( reinterpret_cast<CryptoPP::byte *>( hashed.data( ) ) );
        return hashed;
    }

    auto sha512( string_view_t data ) -> return_string_t
    {
        CryptoPP::SHA512 hash;
        std::string hashed;
        hashed.resize( hash.DigestSize( ) );
        hash.Update( (CryptoPP::byte *)data.data( ), data.size( ) );
        hash.Final( reinterpret_cast<CryptoPP::byte *>( hashed.data( ) ) );
        return hashed;
    }
} // namespace vrock::security