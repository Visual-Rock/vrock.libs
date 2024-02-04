#include "vrock/security.hpp"

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/gcm.h>
#include <cryptopp/modes.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/arc4.h"

#include <stdexcept>
#include <string>
#include <vector>

#include <vrock/utils.hpp>

using namespace vrock::utils;

namespace vrock::security
{
    auto convert_padding_scheme( Padding padding ) -> CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme
    {
        switch ( padding )
        {
        case Padding::NoPadding:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::NO_PADDING;
        case Padding::ZerosPadding:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::ZEROS_PADDING;
        case Padding::OneAndZerosPadding:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::ONE_AND_ZEROS_PADDING;
        case Padding::PkcsPadding:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::PKCS_PADDING;
        case Padding::W3CPadding:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::W3C_PADDING;
        default:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::NO_PADDING;
        }
    }

    auto encrypt_aes_gcm( byte_span_t data, byte_span_t key, byte_span_t iv, byte_span_t authentication_data )
        -> return_t
    {
        return_t cipher;

        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key.data( ), key.size( ), iv.data( ), iv.size( ) );
        CryptoPP::AuthenticatedEncryptionFilter ef( e, new CryptoPP::VectorSink( cipher ), false, 16 );

        ef.ChannelPut( "AAD", authentication_data.data( ), authentication_data.size( ) );
        ef.ChannelMessageEnd( "AAD" );

        ef.ChannelPut( "", data.data( ), data.size( ) );
        ef.ChannelMessageEnd( "" );

        return cipher;
    }

    auto decrypt_aes_gcm( byte_span_t data, byte_span_t key, byte_span_t iv, byte_span_t authentication_data )
        -> return_t
    {
        CryptoPP::GCM<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( key.data( ), key.size( ), iv.data( ), iv.size( ) );
        auto enc = data.subspan( 0, data.size( ) - 16 );
        auto mac = data.subspan( data.size( ) - 16 );

        CryptoPP::AuthenticatedDecryptionFilter df( d, nullptr,
                                                    CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
                                                        CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
                                                    16 );

        df.ChannelPut( "", mac.data( ), mac.size( ) );
        df.ChannelPut( "AAD", authentication_data.data( ), authentication_data.size( ) );
        df.ChannelPut( "", enc.data( ), enc.size( ) );

        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        auto n = df.MaxRetrievable( );
        return_t decrypted;
        decrypted.resize( n );
        if ( n > 0 )
            df.Get( decrypted.data( ), n );

        return decrypted;
    }

    auto encrypt_aes_gcm( string_view_t data, string_view_t key, string_view_t iv, string_view_t authentication_data )
        -> return_string_t
    {
        std::string cipher;

        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( (CryptoPP::byte *)key.data( ), key.size( ), (CryptoPP::byte *)iv.data( ), iv.size( ) );
        CryptoPP::AuthenticatedEncryptionFilter ef( e, new CryptoPP::StringSink( cipher ), false, 16 );

        ef.ChannelPut( "AAD", (CryptoPP::byte *)authentication_data.data( ), authentication_data.size( ) );
        ef.ChannelMessageEnd( "AAD" );

        ef.ChannelPut( "", (CryptoPP::byte *)data.data( ), data.size( ) );
        ef.ChannelMessageEnd( "" );

        return cipher;
    }

    auto decrypt_aes_gcm( string_view_t data, string_view_t key, string_view_t iv, string_view_t authentication_data )
        -> return_string_t
    {
        CryptoPP::GCM<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( (CryptoPP::byte *)key.data( ), key.size( ), (CryptoPP::byte *)iv.data( ), iv.size( ) );

        auto enc = data.substr( 0, data.length( ) - 16 );
        auto mac = data.substr( data.length( ) - 16 );

        CryptoPP::AuthenticatedDecryptionFilter df( d, nullptr,
                                                    CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
                                                        CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
                                                    16 );

        df.ChannelPut( "", (CryptoPP::byte *)mac.data( ), mac.size( ) );
        df.ChannelPut( "AAD", (CryptoPP::byte *)authentication_data.data( ), authentication_data.size( ) );
        df.ChannelPut( "", (CryptoPP::byte *)enc.data( ), enc.size( ) );

        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        auto n = df.MaxRetrievable( );
        return_string_t decrypted;
        decrypted.resize( n );
        if ( n > 0 )
            df.Get( (CryptoPP::byte *)decrypted.data( ), n );

        return decrypted;
    }

    auto encrypt_aes_ecb( byte_span_t data, byte_span_t key, Padding padding ) -> return_t
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption e;
        e.SetKey( key.data( ), key.size( ) );
        return_t cipher;
        CryptoPP::StringSource c( to_string( data ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::VectorSink( cipher ),
                                                                            convert_padding_scheme( padding ) ) );

        return cipher;
    }

    auto decrypt_aes_ecb( byte_span_t data, byte_span_t key, Padding padding ) -> return_t
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption d;
        d.SetKey( key.data( ), key.size( ) );
        return_t decrypted;
        CryptoPP::StringSource s( to_string( data ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::VectorSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );
        return decrypted;
    }

    auto encrypt_aes_ecb( string_view_t data, string_view_t key, Padding padding ) -> return_string_t
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption e;
        e.SetKey( (CryptoPP::byte *)key.data( ), key.size( ) );
        return_string_t cipher;
        CryptoPP::StringSource c( (CryptoPP::byte *)data.data( ), data.size( ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::StringSink( cipher ),
                                                                            convert_padding_scheme( padding ) ) );

        return cipher;
    }

    auto decrypt_aes_ecb( string_view_t data, string_view_t key, Padding padding ) -> return_string_t
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption d;
        d.SetKey( (CryptoPP::byte *)key.data( ), key.size( ) );
        return_string_t decrypted;
        CryptoPP::StringSource s( (CryptoPP::byte *)data.data( ), data.size( ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::StringSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );
        return decrypted;
    }

    auto encrypt_aes_cbc( byte_span_t data, byte_span_t key, byte_span_t iv, Padding padding ) -> return_t
    {
        if ( iv.size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key.data( ), key.size( ), iv.data( ) );
        return_t cipher;
        CryptoPP::StringSource s( to_string( data ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::VectorSink( cipher ),
                                                                            convert_padding_scheme( padding ) ) );
        return cipher;
    }

    auto decrypt_aes_cbc( byte_span_t data, byte_span_t key, byte_span_t iv, Padding padding ) -> return_t
    {
        if ( iv.size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( key.data( ), key.size( ), iv.data( ) );

        return_t decrypted;
        CryptoPP::StringSource s( to_string( data ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::VectorSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );

        return decrypted;
    }

    auto encrypt_aes_cbc( string_view_t data, string_view_t key, string_view_t iv, Padding padding ) -> return_string_t
    {
        if ( iv.size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( (CryptoPP::byte *)key.data( ), key.size( ), (CryptoPP::byte *)iv.data( ) );
        return_string_t cipher;
        CryptoPP::StringSource s( (CryptoPP::byte *)data.data( ), data.size( ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::StringSink( cipher ),
                                                                            convert_padding_scheme( padding ) ) );
        return cipher;
    }

    auto decrypt_aes_cbc( string_view_t data, string_view_t key, string_view_t iv, Padding padding ) -> return_string_t
    {
        if ( iv.size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( (CryptoPP::byte *)key.data( ), key.size( ), (CryptoPP::byte *)iv.data( ) );

        return_string_t decrypted;
        CryptoPP::StringSource s( (CryptoPP::byte *)data.data( ), data.size( ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::StringSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );
        return decrypted;
    }

    auto encrypt_rc4( byte_span_t data, byte_span_t key ) -> return_t
    {
        CryptoPP::Weak::ARC4 rc4;

        if ( rc4.MaxKeyLength( ) < key.size( ) && rc4.MinKeyLength( ) > key.size( ) )
            throw std::invalid_argument( "the key is of invalid length" );

        rc4.SetKey( key.data( ), key.size( ) );
        return_t encr;
        encr.resize( data.size( ) );

        rc4.ProcessData( encr.data( ), data.data( ), data.size( ) );

        return encr;
    }

    auto decrypt_rc4( byte_span_t data, byte_span_t key ) -> return_t
    {
        return encrypt_rc4( data, key );
    }

    auto encrypt_rc4( string_view_t data, string_view_t key ) -> return_string_t
    {
        CryptoPP::Weak::ARC4 rc4;

        if ( rc4.MaxKeyLength( ) < key.size( ) && rc4.MinKeyLength( ) > key.size( ) )
            throw std::invalid_argument( "the key is of invalid length" );

        rc4.SetKey( (CryptoPP::byte *)key.data( ), key.size( ) );
        return_string_t encr;
        encr.resize( data.size( ) );
        rc4.ProcessData( (CryptoPP::byte *)encr.data( ), (CryptoPP::byte *)data.data( ), data.size( ) );

        return encr;
    }

    auto decrypt_rc4( string_view_t data, string_view_t key ) -> return_string_t
    {
        return encrypt_rc4( data, key );
    }
} // namespace vrock::security