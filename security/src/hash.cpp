module;

#include <cryptopp/sha.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/md5.h"

module vrock.security.hash;

namespace vrock::security
{
    auto md5( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::Weak::MD5 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data(), data.size() );
        hash.Final( hashed.data() );
        return hashed;
    }

    auto sha224( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA224 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data(), data.size() );
        hash.Final( hashed.data() );
        return hashed;
    }
    
    auto sha256( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA256 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data(), data.size() );
        hash.Final( hashed.data() );
        return hashed;
    }

    auto sha384( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA384 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data(), data.size() );
        hash.Final( hashed.data() );
        return hashed;
    }

    auto sha512( const utils::ByteArray<> &data ) -> utils::ByteArray<>
    {
        CryptoPP::SHA512 hash;
        auto hashed = utils::ByteArray<>( hash.DigestSize( ) );
        hash.Update( data.data(), data.size() );
        hash.Final( hashed.data() );
        return hashed;
    }
} // namespace vrock::security