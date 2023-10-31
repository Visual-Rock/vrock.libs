#include "vrock/pdf/structure/PDFEncryption.hpp"
#include "vrock/pdf/structure/PDFContext.hpp"

#include <vrock/security.hpp>

#include <unicode/unistr.h>
#include <unicode/usprep.h>
#include <unicode/ustring.h>

#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace vrock::pdf
{
    PDFNullSecurityHandler::PDFNullSecurityHandler( ) : PDFBaseSecurityHandler( SecurityHandlerType::Null )
    {
    }

    auto PDFNullSecurityHandler::is_encrypted( ) -> bool
    {
        return false;
    }

    auto PDFNullSecurityHandler::decrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        return data;
    }

    auto PDFNullSecurityHandler::encrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        return data;
    }

    auto PDFNullSecurityHandler::has_permission( Permissions permissions ) -> bool
    {
        return true;
    }

    PDFStandardSecurityHandler::PDFStandardSecurityHandler( std::shared_ptr<PDFDictionary> encryption_dict,
                                                            std::shared_ptr<PDFContext> ctx, std::function<void( )> f )
        : PDFBaseSecurityHandler( SecurityHandlerType::Standard ), context( std::move( ctx ) ),
          dict( std::move( encryption_dict ) ), fn( std::move( f ) )
    {
        if ( auto r = dict->get<PDFInteger>( "R" ) )
            revision = r->value;
        if ( auto p = dict->get<PDFInteger>( "P" ) )
            permissions = p->value;

        if ( revision == 4 )
        {
            auto cf = dict->get<PDFDictionary>( "CF" );
            if ( cf == nullptr )
                return;
            auto stdcf = cf->get<PDFDictionary>( "StdCF" );
            if ( stdcf == nullptr )
                return;
            if ( auto cfm = stdcf->get<PDFName>( "CFM" ) )
                use_aes = cfm->name == "AESV2";
        }
        if ( revision == 5 || revision == 6 )
            use_aes = true;
    }

    auto PDFStandardSecurityHandler::is_encrypted( ) -> bool
    {
        return true;
    }

    auto PDFStandardSecurityHandler::decrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> ref )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        if ( state == AuthenticationState::Failed )
            throw PDFEncryptedException( "Not Authenticated!" );
        switch ( revision )
        {
        case 2:
        case 3:
        case 4:
            return decrypt_data_1( data, ref, key, use_aes );
        case 5:
        case 6:
            return decrypt_data_1a( data, key );
        case 7: {
            auto iv = data->subarr_shared( 0, 12 );
            auto encrypted = data->subarr_shared( 12, data->size( ) - 12 - 16 );
            auto ad = data->subarr_shared( data->size( ) + 12, 16 );
            return security::decrypt_aes_gcm( encrypted, key, iv, ad );
        }
        default:
            throw PDFEncryptedException( "revision is not supported" );
        }
    }

    auto PDFStandardSecurityHandler::encrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> ref )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        switch ( revision )
        {
        case 2:
        case 3:
        case 4:
            return encrypt_data_1( data, ref, key, use_aes );
        case 5:
        case 6:
            return encrypt_data_1a( data, key );
        case 7: {
            auto iv = security::generate_random_bytes_shared( 12 );
            iv->append( security::encrypt_aes_gcm( data, key, iv ) );
            return iv;
        }
        default:
            throw PDFEncryptedException( "revision is not supported" );
        }
    }

    auto PDFStandardSecurityHandler::has_user_password( ) -> bool
    {
        return authenticate( "" ) == AuthenticationState::Failed;
    }

    auto prep_password( std::string ) -> std::string;

    auto authenticate_user( const std::string &password, uint8_t revision, const std::shared_ptr<PDFDictionary> &dict,
                            std::shared_ptr<utils::ByteArray<>> id, bool *enc_meta )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        switch ( revision )
        {
        case 2:
        case 3:
        case 4:
            return authenticate_user_password_6( password, dict, id );
        case 5:
        case 6:
        case 7:
            return authenticate_user_password_11( prep_password( password ), dict, enc_meta );
        default:
            return nullptr;
        }
    }

    auto authenticate_owner( const std::string &password, uint8_t revision, const std::shared_ptr<PDFDictionary> &dict,
                             std::shared_ptr<utils::ByteArray<>> id, bool *enc_meta )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        switch ( revision )
        {
        case 2:
        case 3:
        case 4:
            return authenticate_owner_password_7( password, dict, id );
        case 5:
        case 6:
        case 7:
            return authenticate_owner_password_12( prep_password( password ), dict, enc_meta );
        default:
            return nullptr;
        }
    }

    auto PDFStandardSecurityHandler::authenticate( const std::string &password ) -> AuthenticationState
    {
        auto i = context->trailer->get<PDFArray>( "ID" );
        if ( i || revision > 4 )
        {
            auto id = i->get<PDFString>( 0 )->get_byte_array( );
            auto k = authenticate_owner( password, revision, dict, id, &encrypt_metadata );
            if ( k != nullptr )
            {
                state = AuthenticationState::Owner;
                key = k;
                fn( );
                return state;
            }
            k = authenticate_user( password, revision, dict, id, &encrypt_metadata );
            if ( k != nullptr )
            {
                state = AuthenticationState::User;
                key = k;
                fn( );
                return state;
            }
        }
        else
            throw std::runtime_error( "required entry ID missing" );
        return AuthenticationState::Failed;
    }

    auto PDFStandardSecurityHandler::is_authenticated( ) -> bool
    {
        return state != AuthenticationState::Failed;
    }

    auto PDFStandardSecurityHandler::has_permission( Permissions perm ) -> bool
    {
        return ( permissions & (std::int32_t)perm ) || state == AuthenticationState::Owner;
    }

    auto get_object_key( std::shared_ptr<PDFRef> ref, std::shared_ptr<utils::ByteArray<>> key, bool use_aes )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto k = std::make_shared<utils::ByteArray<>>( key->size( ) + 5 + ( use_aes ? 4 : 0 ) );
        std::memcpy( k->data( ), key->data( ), key->size( ) );
        k->at( key->size( ) ) = (uint8_t)ref->object_number;
        k->at( key->size( ) + 1 ) = (uint8_t)( ref->object_number >> 8 );
        k->at( key->size( ) + 2 ) = (uint8_t)( ref->object_number >> 16 );
        k->at( key->size( ) + 3 ) = (uint8_t)( ref->generation_number );
        k->at( key->size( ) + 4 ) = (uint8_t)( ref->generation_number >> 8 );
        if ( use_aes )
        {
            k->at( key->size( ) + 5 ) = 0x73; // s
            k->at( key->size( ) + 6 ) = 0x41; // A
            k->at( key->size( ) + 7 ) = 0x6c; // I
            k->at( key->size( ) + 8 ) = 0x54; // T
        }
        return security::md5( k );
    }

    auto encrypt_data_1( std::shared_ptr<utils::ByteArray<>> data, const std::shared_ptr<PDFRef> &ref,
                         std::shared_ptr<utils::ByteArray<>> key, bool use_aes ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto obj_key = get_object_key( ref, key, use_aes );
        obj_key = obj_key->subarr_shared( 0, std::min( key->size( ) + 5, (size_t)16 ) );
        if ( !use_aes )
            return security::encrypt_rc4( data, obj_key );
        auto iv = security::generate_random_bytes_shared( 16 );
        iv->append( security::encrypt_aes_cbc( data, obj_key, iv ) );
        return iv;
    }

    auto encrypt_data_1a( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<utils::ByteArray<>> key )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto iv = security::generate_random_bytes_shared( 16 );
        iv->append( security::encrypt_aes_cbc( data, key, iv ) );
        return iv;
    }

    auto decrypt_data_1( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> ref,
                         std::shared_ptr<utils::ByteArray<>> key, bool use_aes ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto obj_key = get_object_key( ref, key, use_aes );
        obj_key = obj_key->subarr_shared( 0, std::min( key->size( ) + 5, (size_t)16 ) );
        if ( !use_aes )
            return security::decrypt_rc4( data, obj_key );
        auto iv = data->subarr_shared( 0, 16 );
        auto encrypted = data->subarr_shared( 16 );
        return security::decrypt_aes_cbc( encrypted, obj_key, iv );
    }

    auto decrypt_data_1a( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<utils::ByteArray<>> key )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto iv = data->subarr_shared( 0, 16 );
        auto encrypted = data->subarr_shared( 16 );
        return security::decrypt_aes_cbc( encrypted, key, iv );
    }

    void pad_or_truncate_password( std::shared_ptr<utils::ByteArray<>> out, const std::string &pw, size_t pos = 0 )
    {
        if ( pw.length( ) < 32 )
        {
            std::memcpy( out->data( ) + pos, pw.data( ), pw.size( ) );
            std::memcpy( out->data( ) + pw.size( ) + pos, fill->data( ), 32 - pw.size( ) );
        }
        else
            std::memcpy( out->data( ) + pos, pw.data( ), 32 );
    }

    auto compute_file_encryption_key_2( const std::string &password, const std::shared_ptr<utils::ByteArray<>> &o,
                                        std::uint32_t p, const std::shared_ptr<utils::ByteArray<>> &id,
                                        std::int32_t length, std::uint8_t revision, bool encrypt_metadata )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        const auto ff4 = utils::from_hex_string( "ffffffff" );
        // a, b)
        auto pw = std::make_shared<utils::ByteArray<>>( 68 + id->size( ) + ( !encrypt_metadata * 4 ) );
        pad_or_truncate_password( pw, password );
        // c)
        std::memcpy( pw->data( ) + 32, o->data( ), o->size( ) );
        // d)p
        ( *pw )[ 64 ] = (uint8_t)p;
        ( *pw )[ 65 ] = (uint8_t)( p >> 8 );
        ( *pw )[ 66 ] = (uint8_t)( p >> 16 );
        ( *pw )[ 67 ] = (uint8_t)( p >> 24 );
        // e)
        std::memcpy( pw->data( ) + 68, id->data( ), id->size( ) );
        // f)
        if ( !encrypt_metadata )
            std::memcpy( pw->data( ) + 68 + id->size( ), ff4.data( ), 4 );

        // g)
        auto hash = security::md5( pw );
        // h)
        if ( revision >= 3 )
        {
            auto ret = std::make_shared<utils::ByteArray<>>( length / 8 );
            std::memcpy( ret->data( ), hash->data( ), ret->size( ) );
            for ( int j = 0; j < 50; ++j )
            {
                hash = security::md5( ret );
                std::memcpy( ret->data( ), hash->data( ), ret->size( ) );
            }
            return ret;
        }
        // i)
        else
        {
            auto ret = std::make_shared<utils::ByteArray<>>( 5 );
            std::memcpy( ret->data( ), hash->data( ), 5 );
            return ret;
        }
    }

    auto retrieve_file_encryption_key_2a( const std::string &password, AuthenticationState state,
                                          std::shared_ptr<utils::ByteArray<>> o, std::shared_ptr<utils::ByteArray<>> u,
                                          std::shared_ptr<utils::ByteArray<>> oe,
                                          std::shared_ptr<utils::ByteArray<>> ue, std::int32_t p,
                                          std::shared_ptr<utils::ByteArray<>> perms, std::uint8_t revision,
                                          bool *out_encrypt_metadata ) -> std::shared_ptr<utils::ByteArray<>>
    {
        std::shared_ptr<utils::ByteArray<>> key;
        if ( state == AuthenticationState::Owner )
            key = security::decrypt_aes_cbc( oe, compute_hash_2b( password, o->subarr_shared( 40, 8 ), u, revision ),
                                             std::make_shared<utils::ByteArray<>>( 16 ) );
        else
            key = security::decrypt_aes_cbc( ue,
                                             compute_hash_2b( password, u->subarr_shared( 40, 8 ), nullptr, revision ),
                                             std::make_shared<utils::ByteArray<>>( 16 ) );
        auto decrypted_perm = security::decrypt_aes_ecb( perms, key );
        if ( decrypted_perm->at( 9 ) != 'a' || decrypted_perm->at( 10 ) != 'd' || decrypted_perm->at( 11 ) != 'b' )
            throw std::runtime_error( "perms byte 9 to 11 do not match adb" );
        std::int16_t p_calc = decrypted_perm->at( 0 );
        p_calc |= (std::int16_t)decrypted_perm->at( 1 ) << 8;  // NOLINT
        p_calc |= (std::int16_t)decrypted_perm->at( 2 ) << 16; // NOLINT
        if ( p_calc != p )
            throw std::runtime_error( "p does not match decrypted perm byte 0 to 3" );
        *out_encrypt_metadata = decrypted_perm->at( 8 ) == 'T';
        return key;
    }

    thread_local auto k1 = std::make_shared<utils::ByteArray<>>( 64 * ( 127 + 64 + 48 ) );
    auto compute_hash_2b( const std::string &password, std::shared_ptr<utils::ByteArray<>> salt,
                          std::shared_ptr<utils::ByteArray<>> user_key, std::uint8_t revision )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        std::shared_ptr<utils::ByteArray<>> k;
        std::shared_ptr<utils::ByteArray<>> t;
        std::shared_ptr<utils::ByteArray<>> e;
        std::memset( k1->data( ), 0, k1->size( ) );
        auto com_key = std::make_shared<utils::ByteArray<>>( 16 );
        auto iv = std::make_shared<utils::ByteArray<>>( 16 );
        auto num = std::make_shared<utils::ByteArray<>>( 16 );

        auto pw_len = password.length( ) > 127 ? 127 : password.length( );
        std::size_t hash_len = 32;
        auto uk_len = user_key != nullptr ? user_key->size( ) : 0;
        std::size_t seq_len = 0;
        std::size_t len = 0;
        int rounds = 0;
        std::uint64_t rem;

        if ( salt != nullptr )
        {
            auto h = std::make_shared<utils::ByteArray<>>( password.length( ) + salt->size( ) + uk_len );
            memcpy( h->data( ), password.data( ), pw_len );
            memcpy( h->data( ) + pw_len, salt->data( ), salt->size( ) );
            if ( user_key != nullptr )
                memcpy( h->data( ) + pw_len + salt->size( ), user_key->data( ), uk_len );
            k = security::sha256( h );
        }
        else
            k = security::sha256( std::make_shared<utils::ByteArray<>>( password ) );

        if ( revision == 5 )
            return k;

        while ( rounds < 64 || rounds < e->at( len - 1 ) + 32 )
        {
            // a)
            seq_len = pw_len + hash_len + uk_len;
            len = 64 * seq_len;
            memcpy( k1->data( ), password.data( ), pw_len );
            memcpy( k1->data( ) + pw_len, k->data( ), hash_len );
            if ( user_key != nullptr )
                memcpy( k1->data( ) + pw_len + +hash_len, user_key->data( ), uk_len );
            for ( size_t i = 1; i < 64; i++ )
                memcpy( k1->data( ) + ( i * seq_len ), k1->data( ), seq_len );
            // b)
            t = std::make_shared<utils::ByteArray<>>( len );
            memcpy( com_key->data( ), k->data( ), 16 );
            memcpy( iv->data( ), k->data( ) + 16, 16 );
            memcpy( t->data( ), k1->data( ), len );
            e = security::encrypt_aes_cbc( t, com_key, iv );
            // c)
            memcpy( num->data( ), e->data( ), 16 );
            uint64_t n1 = 0, n2 = 0, n3 = 0;
            n1 |= (uint64_t)num->at( 0 ) << 56;
            n1 |= (uint64_t)num->at( 1 ) << 48;
            n1 |= (uint64_t)num->at( 2 ) << 40;
            n1 |= (uint64_t)num->at( 3 ) << 32;
            n1 |= (uint64_t)num->at( 4 ) << 24;
            n1 |= (uint64_t)num->at( 5 ) << 16;
            n1 |= (uint64_t)num->at( 6 ) << 8;
            n1 |= (uint64_t)num->at( 7 );
            rem = n1 % 3;
            n2 |= (uint64_t)num->at( 8 ) << 24;
            n2 |= (uint64_t)num->at( 9 ) << 16;
            n2 |= (uint64_t)num->at( 10 ) << 8;
            n2 |= (uint64_t)num->at( 11 );
            rem = ( n2 | rem << 32 ) % 3;
            n3 |= (uint64_t)num->at( 12 ) << 24;
            n3 |= (uint64_t)num->at( 13 ) << 16;
            n3 |= (uint64_t)num->at( 14 ) << 8;
            n3 |= (uint64_t)num->at( 15 );
            rem = ( n3 | rem << 32 ) % 3;

            if ( rem == 0 )
                k = security::sha256( e );
            else if ( rem == 1 )
                k = security::sha384( e );
            else
                k = security::sha512( e );
            hash_len = k->size( );
            rounds++;
        }
        auto ret = std::make_shared<utils::ByteArray<>>( 32 );
        memcpy( ret->data( ), k->data( ), 32 );
        return ret;
    }

    auto compute_o_3( const std::string &owner_password, const std::string &user_password, std::int32_t length,
                      std::uint8_t revision ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto n = revision == 2 ? 5 : length / 8;
        // a)
        std::string pw_str = owner_password.empty( ) ? user_password : owner_password;
        auto pw = std::make_shared<utils::ByteArray<>>( 32 );
        pad_or_truncate_password( pw, pw_str );
        // b)
        auto hash_1 = security::md5( pw );
        // c)
        if ( revision >= 3 )
            for ( int j = 0; j < 50; ++j )
                hash_1 = security::md5( hash_1 );
        // d)
        auto key_com = std::make_shared<utils::ByteArray<>>( n );
        std::memcpy( key_com->data( ), hash_1->data( ), n );
        // e)
        pw_str = user_password;
        pad_or_truncate_password( pw, pw_str );
        // f)
        auto encrypted = security::encrypt_rc4( pw, key_com );
        // g)
        auto key = std::make_shared<utils::ByteArray<>>( key_com->size( ) );
        if ( revision >= 3 )
        {
            for ( int i = 1; i < 20; ++i )
            {
                memcpy( key->data( ), key_com->data( ), key->size( ) );
                for ( size_t j = 0; j < key->size( ); ++j )
                    ( *key )[ j ] ^= i;
                encrypted = security::encrypt_rc4( encrypted, key );
            }
        }
        return encrypted;
    }

    auto compute_u_4( std::shared_ptr<utils::ByteArray<>> key ) -> std::shared_ptr<utils::ByteArray<>>
    {
        return security::encrypt_rc4( fill, key );
    }

    auto compute_u_5( std::shared_ptr<utils::ByteArray<>> key, std::shared_ptr<utils::ByteArray<>> id )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto h = std::make_shared<utils::ByteArray<>>( 32 + id->size( ) );
        std::memcpy( h->data( ), fill->data( ), 32 );
        std::memcpy( h->data( ) + 32, id->data( ), id->size( ) );
        auto pad_hash = security::md5( h );
        // d)
        auto encrypted = security::encrypt_rc4( pad_hash, key );
        // e)
        std::shared_ptr<utils::ByteArray<>> new_key = std::make_shared<utils::ByteArray<>>( key->size( ) );
        for ( int k = 1; k <= 19; k++ )
        {
            std::memcpy( new_key->data( ), key->data( ), key->size( ) );
            for ( size_t j = 0; j < new_key->size( ); ++j )
                new_key->at( j ) ^= k;
            encrypted = security::encrypt_rc4( encrypted, new_key );
        }
        // f)
        auto u = std::make_shared<utils::ByteArray<>>( 32 );
        std::memcpy( u->data( ), encrypted->data( ), encrypted->size( ) );
        std::memcpy( u->data( ) + 16, fill->data( ), 16 ); // Pad with arbitrary data
        return u;
    }

    auto authenticate_user_password_6( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                       std::shared_ptr<utils::ByteArray<>> id ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto len = dict->get<PDFInteger>( "Length" );
        auto u = dict->get<PDFString>( "U" ); // 32 byte
        auto r = dict->get<PDFInteger>( "R" );
        auto o = dict->get<PDFString>( "O" );  // 32 byte
        auto p = dict->get<PDFInteger>( "P" ); // 4 byte
        auto enc_meta = dict->get<PDFBool>( "EncryptMetadata" );

        if ( !( o && p && len && r ) )
            return nullptr;

        std::shared_ptr<utils::ByteArray<>> u_com;
        auto key = compute_file_encryption_key_2( password, o->get_byte_array( ), p->value, id, len->value, r->value,
                                                  !( enc_meta != nullptr ) || enc_meta );
        switch ( r->value )
        {
        case 2:
            u_com = compute_u_4( key );
            break;
        case 3:
        case 4:
            u_com = compute_u_5( key, id );
            break;
        default:
            // log::get_logger( "pdf" )->log->info( "revision {} is not supported!", r );
            return nullptr;
        }
        if ( u_com->to_string( ).substr( 0, 16 ) == u->get_byte_array( )->to_string( ).substr( 0, 16 ) )
            return key;
        return nullptr;
    }

    auto authenticate_owner_password_7( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        std::shared_ptr<utils::ByteArray<>> id ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto len = dict->get<PDFInteger>( "Length" );
        auto r = dict->get<PDFInteger>( "R" );
        auto o = dict->get<PDFString>( "O" ); // 32 byte

        if ( !( len && r && o ) )
            return nullptr;
        auto n = r->value == 2 ? 5 : len->value / 8;
        // a)
        auto pw = std::make_shared<utils::ByteArray<>>( 32 );
        pad_or_truncate_password( pw, password );
        auto hash_1 = security::md5( pw );
        if ( r->value >= 3 )
            for ( int j = 0; j < 50; ++j )
                hash_1 = security::md5( hash_1 );
        auto key = std::make_shared<utils::ByteArray<>>( n );
        std::memcpy( key->data( ), hash_1->data( ), n );
        // b)
        std::shared_ptr<utils::ByteArray<>> u;
        if ( r->value == 2 )
            u = security::decrypt_rc4( o->get_byte_array( ), key );
        else
        {
            u = o->get_byte_array( );
            auto com_key = std::make_shared<utils::ByteArray<>>( key->size( ) );
            for ( int i = 19; i >= 0; i-- )
            {
                memcpy( com_key->data( ), key->data( ), key->size( ) );
                for ( size_t j = 0; j < com_key->size( ); j++ )
                    com_key->at( j ) ^= i;
                u = security::decrypt_rc4( u, com_key );
            }
        }
        return authenticate_user_password_6( u->to_string( ), dict, std::move( id ) );
    }

    auto compute_u_ue_8( const std::string &password, std::shared_ptr<utils::ByteArray<>> key, uint8_t revision )
        -> std::pair<std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<utils::ByteArray<>>>
    {
        auto u = std::make_shared<utils::ByteArray<>>( 48 );
        std::shared_ptr<utils::ByteArray<>> ue;
        // a)
        auto user_validation_salt = security::generate_random_bytes_shared( 8 );
        auto user_key_salt = security::generate_random_bytes_shared( 8 );
        {
            auto hash = compute_hash_2b( password, user_validation_salt, nullptr, revision );
            // TODO: update
            memcpy( u->data( ), hash->data( ), 32 );
            memcpy( u->data( ) + 32, user_validation_salt->data( ), 8 );
            memcpy( u->data( ) + 40, user_key_salt->data( ), 8 );
        }
        {
            auto hash = compute_hash_2b( password, user_key_salt, nullptr, revision );
            ue = security::encrypt_aes_cbc( key, hash, std::make_shared<utils::ByteArray<>>( 16 ) );
        }

        return { u, ue };
    }

    auto compute_o_oe_9( const std::string &password, std::shared_ptr<utils::ByteArray<>> key,
                         std::shared_ptr<utils::ByteArray<>> u, uint8_t revision )
        -> std::pair<std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<utils::ByteArray<>>>
    {
        auto o = std::make_shared<utils::ByteArray<>>( 48 );
        std::shared_ptr<utils::ByteArray<>> oe;
        // a)
        auto owner_validation_salt = security::generate_random_bytes_shared( 8 );
        auto owner_key_salt = security::generate_random_bytes_shared( 8 );
        {
            auto hash = compute_hash_2b( password, owner_validation_salt, u, revision );
            // TODO: update
            memcpy( o->data( ), hash->data( ), 32 );
            memcpy( o->data( ) + 32, owner_validation_salt->data( ), 8 );
            memcpy( o->data( ) + 40, owner_key_salt->data( ), 8 );
        }
        {
            auto hash = compute_hash_2b( password, owner_key_salt, u, revision );
            oe = security::encrypt_aes_cbc( key, hash, std::make_shared<utils::ByteArray<>>( 16 ) );
        }
        return { o, oe };
    }

    auto encrypt_perms_10( std::int32_t p, bool enc_meta_data, std::shared_ptr<utils::ByteArray<>> key )
        -> std::shared_ptr<utils::ByteArray<>>
    {
        auto perm = std::make_shared<utils::ByteArray<>>( 16 );
        perm->at( 0 ) = (uint8_t)p;
        perm->at( 1 ) = (uint8_t)( p >> 8 );
        perm->at( 2 ) = (uint8_t)( p >> 16 );
        perm->at( 3 ) = (uint8_t)( p >> 24 );
        std::memset( perm->data( ) + 4, 0xff, 4 ); // higher order 32 bit to 1
        perm->at( 8 ) = enc_meta_data ? 'T' : 'F';
        perm->at( 9 ) = 'a';
        perm->at( 10 ) = 'd';
        perm->at( 11 ) = 'b';
        auto rand = security::generate_random_bytes( 4 );
        std::memcpy( perm->data( ) + 12, rand.data( ), 4 );
        return security::encrypt_aes_ecb( perm, key );
    }

    auto authenticate_user_password_11( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        bool *out_encrypt_meta ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto u = dict->get<PDFString>( "U" );
        auto o = dict->get<PDFString>( "O" );
        auto ue = dict->get<PDFString>( "UE" );
        auto oe = dict->get<PDFString>( "OE" );
        auto p = dict->get<PDFInteger>( "P" );
        auto perm = dict->get<PDFString>( "Perms" );
        auto r = dict->get<PDFInteger>( "R" );
        if ( !( u && ue && o && oe && p && perm && r ) )
            return nullptr;
        // a)
        auto d = std::make_shared<utils::ByteArray<>>( 8 );
        memcpy( d->data( ), u->get_byte_array( )->to_string( ).data( ) + 32, 8 );
        auto hash = compute_hash_2b( password, d, nullptr, r->value );
        if ( u->get_byte_array( )->to_string( ).substr( 0, 32 ) != hash->to_string( ) )
            return nullptr;
        return retrieve_file_encryption_key_2a( password, AuthenticationState::User, o->get_byte_array( ),
                                                u->get_byte_array( ), oe->get_byte_array( ), ue->get_byte_array( ),
                                                p->value, perm->get_byte_array( ), r->value, out_encrypt_meta );
    }

    auto authenticate_owner_password_12( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                         bool *out_encrypt_meta ) -> std::shared_ptr<utils::ByteArray<>>
    {
        auto u = dict->get<PDFString>( "U" );
        auto o = dict->get<PDFString>( "O" );
        auto ue = dict->get<PDFString>( "UE" );
        auto oe = dict->get<PDFString>( "OE" );
        auto p = dict->get<PDFInteger>( "P" );
        auto perm = dict->get<PDFString>( "Perms" );
        auto r = dict->get<PDFInteger>( "R" );
        if ( !( u && ue && o && oe && p && perm && r ) )
            return nullptr;
        auto o_str = o->get_byte_array( )->subarr_shared( 0, 32 )->to_string( );
        auto hash =
            compute_hash_2b( password, o->get_byte_array( )->subarr_shared( 32, 8 ), u->get_byte_array( ), r->value );
        if ( o_str != hash->to_string( ) )
            return nullptr;
        return retrieve_file_encryption_key_2a( password, AuthenticationState::Owner, o->get_byte_array( ),
                                                u->get_byte_array( ), oe->get_byte_array( ), ue->get_byte_array( ),
                                                p->value, perm->get_byte_array( ), r->value, out_encrypt_meta );
    }

    auto prep_password( std::string str ) -> std::string
    {
        if ( str == "" )
            return str;

        UErrorCode err = U_ZERO_ERROR;
        auto profile = usprep_openByType( USPREP_RFC4013_SASLPREP, &err );
        if ( err > U_ZERO_ERROR )
        {
            //  log::get_logger( "pdf" )->log->info( "string_prep_open failed Error: {0}, {1}", 0, u_errorName( err
            //  ) );
            return str;
        }
        err = U_ZERO_ERROR;
        auto piece = icu::StringPiece( str.c_str( ), str.length( ) );
        auto ustr = icu::UnicodeString::fromUTF8( piece );
        auto len = usprep_prepare( profile, ustr.getBuffer( ), ustr.length( ), nullptr, 0, 0, nullptr, &err );
        err = U_ZERO_ERROR;
        auto res = icu::UnicodeString( len );
        len = usprep_prepare( profile, ustr.getBuffer( ), ustr.length( ), (UChar *)res.getBuffer( ), res.getCapacity( ),
                              0, nullptr, &err );
        if ( err > U_ZERO_ERROR )
        {
            // log::get_logger( "pdf" )->log->info( "string_prep failed Error: {0}, {1}", 0, u_errorName( err ) );
            return str;
        }
        usprep_close( profile );

        auto c = new char[ 127 ];
        std::int32_t amount = -1;
        u_strToUTF8( c, 127, &amount, (UChar *)res.getBuffer( ), len, &err );
        std::string retstr( c );
        delete[] c;
        return retstr;
    }

    template <>
    auto to_handler_type<PDFNullSecurityHandler>( ) -> SecurityHandlerType
    {
        return SecurityHandlerType::Null;
    }

    template <>
    auto to_handler_type<PDFStandardSecurityHandler>( ) -> SecurityHandlerType
    {
        return SecurityHandlerType::Standard;
    }
} // namespace vrock::pdf