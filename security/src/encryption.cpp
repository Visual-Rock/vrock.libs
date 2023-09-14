module;

import vrock.utils.ByteArray;

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/gcm.h>
#include <cryptopp/modes.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/arc4.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

module vrock.security.encryption;

namespace vrock::security
{
    auto convert_padding_scheme( Padding padding ) -> CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme
    {
        switch ( padding )
        {
        case Padding::NO_PADDING:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::NO_PADDING;
        case Padding::ZEROS_PADDING:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::ZEROS_PADDING;
        case Padding::ONE_AND_ZEROS_PADDING:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::ONE_AND_ZEROS_PADDING;
        case Padding::PKCS_PADDING:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::PKCS_PADDING;
        case Padding::W3C_PADDING:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::W3C_PADDING;
        default:
            return CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::NO_PADDING;
        }
    }

    auto encrypt_aes_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key, const utils::ByteArray<> &iv,
                          const utils::ByteArray<> &authentication_data ) -> utils::ByteArray<>
    {
        std::string cipher;

        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key.data( ), key.size( ), iv.data( ), iv.size( ) );
        CryptoPP::AuthenticatedEncryptionFilter ef( e, new CryptoPP::StringSink( cipher ), false, 16 );

        ef.ChannelPut( "AAD", authentication_data.data( ), authentication_data.size( ) );
        ef.ChannelMessageEnd( "AAD" );

        ef.ChannelPut( "", data.data( ), data.size( ) );
        ef.ChannelMessageEnd( "" );

        return utils::ByteArray<>( cipher );
    }

    auto decrypt_aes_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key, const utils::ByteArray<> &iv,
                          const utils::ByteArray<> &authentication_data ) -> utils::ByteArray<>
    {
        CryptoPP::GCM<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( key.data( ), key.size( ), iv.data( ), iv.size( ) );
        auto s = data.to_string( );
        std::string enc = s.substr( 0, s.length( ) - 16 );
        std::string mac = s.substr( s.length( ) - 16 );

        CryptoPP::AuthenticatedDecryptionFilter df( d, nullptr,
                                                    CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
                                                        CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
                                                    16 );

        df.ChannelPut( "", (uint8_t *)mac.data( ), mac.size( ) );
        df.ChannelPut( "AAD", authentication_data.data( ), authentication_data.size( ) );
        df.ChannelPut( "", (uint8_t *)enc.data( ), enc.size( ) );

        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        auto n = df.MaxRetrievable( );
        auto decrypted = utils::ByteArray<>( n );
        if ( n > 0 )
            df.Get( decrypted.data( ), n );

        return decrypted;
    }

    auto encrypt_aes_ecb( const utils::ByteArray<> &data, const utils::ByteArray<> &key, Padding padding )
        -> utils::ByteArray<>
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption e;
        e.SetKey( key.data( ), key.size( ) );
        std::string cypher;
        CryptoPP::StringSource c( data.to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::StringSink( cypher ),
                                                                            convert_padding_scheme( padding ) ) );

        return utils::ByteArray<>( cypher );
    }

    auto decrypt_aes_ecb( const utils::ByteArray<> &data, const utils::ByteArray<> &key, Padding padding )
        -> utils::ByteArray<>
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption d;
        d.SetKey( key.data( ), key.size( ) );
        std::string decrypted;

        CryptoPP::StringSource s( data.to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::StringSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );

        return utils::ByteArray<>( decrypted );
    }

    auto encrypt_aes_cbc( const utils::ByteArray<> &data, const utils::ByteArray<> &key, const utils::ByteArray<> &iv,
                          Padding padding ) -> utils::ByteArray<>
    {
        if ( iv.size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key.data( ), key.size( ), iv.data( ) );
        std::string cipher;
        CryptoPP::StringSource s( data.to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::StringSink( cipher ),
                                                                            convert_padding_scheme( padding ) ) );
        return utils::ByteArray<>( cipher );
    }

    auto decrypt_aes_cbc( const utils::ByteArray<> &data, const utils::ByteArray<> &key, const utils::ByteArray<> &iv,
                          Padding padding ) -> utils::ByteArray<>
    {
        if ( iv.size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( key.data( ), key.size( ), iv.data( ) );

        std::string decrypted;

        CryptoPP::StringSource s( data.to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::StringSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );

        return utils::ByteArray<>( decrypted );
    }

    auto encrypt_rc4( const utils::ByteArray<> &data, const utils::ByteArray<> &key ) -> utils::ByteArray<>
    {
        CryptoPP::Weak::ARC4 rc4;

        if ( rc4.MaxKeyLength( ) < key.size( ) && rc4.MinKeyLength( ) > key.size( ) )
            throw std::invalid_argument( "the key was of invalid length" );

        rc4.SetKey( key.data( ), key.size( ) );
        auto encr = utils::ByteArray<>( data.size( ) );

        rc4.ProcessData( encr.data( ), data.data( ), data.size( ) );

        return encr;
    }

    auto decrypt_rc4( const utils::ByteArray<> &data, const utils::ByteArray<> &key ) -> utils::ByteArray<>
    {
        return encrypt_rc4( data, key );
    }

    auto encrypt_aes_gcm( const std::shared_ptr<utils::ByteArray<>> &data,
                          const std::shared_ptr<utils::ByteArray<>> &key, const std::shared_ptr<utils::ByteArray<>> &iv,
                          const std::shared_ptr<utils::ByteArray<>> &authentication_data )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        std::string cipher;

        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key->data( ), key->size( ), iv->data( ), iv->size( ) );
        CryptoPP::AuthenticatedEncryptionFilter ef( e, new CryptoPP::StringSink( cipher ), false, 16 );

        ef.ChannelPut( "AAD", authentication_data->data( ), authentication_data->size( ) );
        ef.ChannelMessageEnd( "AAD" );

        ef.ChannelPut( "", data->data( ), data->size( ) );
        ef.ChannelMessageEnd( "" );

        return std::make_shared<utils::ByteArray<>>( cipher );
    }

    auto decrypt_aes_gcm( const std::shared_ptr<utils::ByteArray<>> &data,
                          const std::shared_ptr<utils::ByteArray<>> &key, const std::shared_ptr<utils::ByteArray<>> &iv,
                          const std::shared_ptr<utils::ByteArray<>> &authentication_data )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::GCM<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( key->data( ), key->size( ), iv->data( ), iv->size( ) );
        auto s = data->to_string( );
        std::string enc = s.substr( 0, s.length( ) - 16 );
        std::string mac = s.substr( s.length( ) - 16 );

        CryptoPP::AuthenticatedDecryptionFilter df( d, nullptr,
                                                    CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
                                                        CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
                                                    16 );

        df.ChannelPut( "", (uint8_t *)mac.data( ), mac.size( ) );
        df.ChannelPut( "AAD", authentication_data->data( ), authentication_data->size( ) );
        df.ChannelPut( "", (uint8_t *)enc.data( ), enc.size( ) );

        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        auto n = df.MaxRetrievable( );
        auto decrypted = std::make_shared<utils::ByteArray<>>( n );
        if ( n > 0 )
            df.Get( decrypted->data( ), n );

        return decrypted;
    }

    auto encrypt_aes_ecb( const std::shared_ptr<utils::ByteArray<>> &data,
                          const std::shared_ptr<utils::ByteArray<>> &key, Padding padding )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption e;
        e.SetKey( key->data( ), key->size( ) );
        std::string cypher;
        CryptoPP::StringSource c( data->to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::StringSink( cypher ),
                                                                            convert_padding_scheme( padding ) ) );

        return std::make_shared<utils::ByteArray<>>( cypher );
    }

    auto decrypt_aes_ecb( const std::shared_ptr<utils::ByteArray<>> &data,
                          const std::shared_ptr<utils::ByteArray<>> &key, Padding padding )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption d;
        d.SetKey( key->data( ), key->size( ) );
        std::string decrypted;

        CryptoPP::StringSource s( data->to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::StringSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );

        return std::make_shared<utils::ByteArray<>>( decrypted );
    }

    auto encrypt_aes_cbc( const std::shared_ptr<utils::ByteArray<>> &data,
                          const std::shared_ptr<utils::ByteArray<>> &key, const std::shared_ptr<utils::ByteArray<>> &iv,
                          Padding padding ) -> std::shared_ptr<utils::ByteArray<>>
    {
        if ( iv->size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key->data( ), key->size( ), iv->data( ) );
        std::string cipher;
        CryptoPP::StringSource s( data->to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( e, new CryptoPP::StringSink( cipher ),
                                                                            convert_padding_scheme( padding ) ) );
        return std::make_shared<utils::ByteArray<>>( cipher );
    }

    auto decrypt_aes_cbc( const std::shared_ptr<utils::ByteArray<>> &data,
                          const std::shared_ptr<utils::ByteArray<>> &key, const std::shared_ptr<utils::ByteArray<>> &iv,
                          Padding padding ) -> std::shared_ptr<utils::ByteArray<>>
    {
        if ( iv->size( ) != 16 )
            throw std::invalid_argument( "initialization vector has to have a length of 16 bytes" );

        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV( key->data( ), key->size( ), iv->data( ) );

        std::string decrypted;

        CryptoPP::StringSource s( data->to_string( ), true,
                                  new CryptoPP::StreamTransformationFilter( d, new CryptoPP::StringSink( decrypted ),
                                                                            convert_padding_scheme( padding ) ) );

        return std::make_shared<utils::ByteArray<>>( decrypted );
    }

    auto encrypt_rc4( const std::shared_ptr<utils::ByteArray<>> &data, const std::shared_ptr<utils::ByteArray<>> &key )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        CryptoPP::Weak::ARC4 rc4;

        if ( rc4.MaxKeyLength( ) < key->size( ) && rc4.MinKeyLength( ) > key->size( ) )
            throw std::invalid_argument( "the key was of invalid length" );

        rc4.SetKey( key->data( ), key->size( ) );
        auto encr = std::make_shared<utils::ByteArray<>>( data->size( ) );

        rc4.ProcessData( encr->data( ), data->data( ), data->size( ) );

        return encr;
    }

    auto decrypt_rc4( const std::shared_ptr<utils::ByteArray<>> &data, const std::shared_ptr<utils::ByteArray<>> &key )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        return encrypt_rc4( data, key );
    }
} // namespace vrock::security