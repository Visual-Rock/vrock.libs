module;

import <cryptopp/aes.h>;
import <cryptopp/filters.h>;
import <cryptopp/gcm.h>;
import <cryptopp/modes.h>;

import <string>;
import <vector>;
import <iostream>;

export import vrock.utils.ByteArray;

export module vrock.security.encryption:aes;

namespace vrock::security::encryption
{
    export enum class Padding
    {
        NO_PADDING,
        ZEROS_PADDING,
        PKCS_PADDING,
        W3C_PADDING,
        ONE_AND_ZEROS_PADDING
    };

    export auto encrypt_gcm( const utils::ByteArray<> &data, const utils::ByteArray<> &key,
                             const utils::ByteArray<> &iv,
                             const utils::ByteArray<> &authentication_data )
        -> utils::ByteArray<>
    {
        std::string cipher;

        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV( key.data( ), key.size( ), iv.data( ), iv.size( ) );
        CryptoPP::AuthenticatedEncryptionFilter ef( e, new CryptoPP::StringSink( cipher ), false, 16 );

        ef.ChannelPut( "AAD", authentication_data.data( ), authentication_data.size( ) );
        ef.ChannelMessageEnd( "AAD" );

        ef.ChannelPut( "", data.data( ), data.size( ) );
        ef.ChannelMessageEnd( "" );

        return utils::ByteArray( cipher );
    }
}