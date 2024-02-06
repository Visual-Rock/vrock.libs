#include "vrock/pdf/structure/PDFEncryption.hpp"
#include "vrock/pdf/structure/PDFContext.hpp"

#include <vrock/log.hpp>
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

    auto PDFNullSecurityHandler::decrypt( in_data_t data, std::shared_ptr<PDFRef> ) -> data_t
    {
        return data_t( data );
    }

    auto PDFNullSecurityHandler::encrypt( in_data_t data, std::shared_ptr<PDFRef> ) -> data_t
    {
        return data_t( data );
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

    auto PDFStandardSecurityHandler::decrypt( in_data_t data, std::shared_ptr<PDFRef> ref ) -> data_t
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
            auto iv = data.substr( 0, 12 );
            auto encrypted = data.substr( 12, data.size( ) - 12 - 16 );
            auto ad = data.substr( data.size( ) + 12, 16 );
            return security::decrypt_aes_gcm( encrypted, key, iv, ad );
        }
        default:
            throw PDFEncryptedException( "revision is not supported" );
        }
    }

    auto PDFStandardSecurityHandler::encrypt( in_data_t data, std::shared_ptr<PDFRef> ref ) -> data_t
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
            auto rand = security::generate_random_bytes( 12 );
            auto iv = utils::to_string( rand );
            iv.append( security::encrypt_aes_gcm( data, key, iv, "" ) );
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
                            in_data_t id, bool *enc_meta ) -> data_t
    {
        switch ( revision )
        {
        case 2:
        case 3:
        case 4:
            return data_t( authenticate_user_password_6( password, dict, id ) );
        case 5:
        case 6:
        case 7:
            return authenticate_user_password_11( prep_password( password ), dict, enc_meta );
        default:
            return "";
        }
    }

    auto authenticate_owner( const std::string &password, uint8_t revision, const std::shared_ptr<PDFDictionary> &dict,
                             in_data_t id, bool *enc_meta ) -> data_t
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
            return "";
        }
    }

    auto PDFStandardSecurityHandler::authenticate( const std::string &password ) -> AuthenticationState
    {
        auto i = context->trailer->get<PDFArray>( "ID" );
        if ( i || revision > 4 )
        {
            auto id = i->get<PDFString>( 0 )->get_data( );
            auto k = authenticate_owner( password, revision, dict, id, &encrypt_metadata );
            if ( !k.empty( ) )
            {
                state = AuthenticationState::Owner;
                key = k;
                fn( );
                return state;
            }
            k = authenticate_user( password, revision, dict, id, &encrypt_metadata );
            if ( !k.empty( ) )
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

    auto get_object_key( std::shared_ptr<PDFRef> ref, in_data_t key, bool use_aes ) -> data_t
    {
        auto k = data_t( key.size( ) + 5 + ( use_aes ? 4 : 0 ), '\0' );
        std::memcpy( k.data( ), key.data( ), key.size( ) );
        k.at( key.size( ) ) = static_cast<uint8_t>( ref->object_number );
        k.at( key.size( ) + 1 ) = static_cast<uint8_t>( ref->object_number >> 8 );
        k.at( key.size( ) + 2 ) = static_cast<uint8_t>( ref->object_number >> 16 );
        k.at( key.size( ) + 3 ) = static_cast<uint8_t>( ref->generation_number );
        k.at( key.size( ) + 4 ) = static_cast<uint8_t>( ref->generation_number >> 8 );
        if ( use_aes )
        {
            k.at( key.size( ) + 5 ) = 0x73; // s
            k.at( key.size( ) + 6 ) = 0x41; // A
            k.at( key.size( ) + 7 ) = 0x6c; // I
            k.at( key.size( ) + 8 ) = 0x54; // T
        }
        return security::md5( k );
    }

    auto encrypt_data_1( in_data_t data, const std::shared_ptr<PDFRef> &ref, in_data_t key, bool use_aes ) -> data_t
    {
        auto obj_key = get_object_key( ref, key, use_aes );
        obj_key = obj_key.substr( 0, std::min( key.size( ) + 5, (size_t)16 ) );
        if ( !use_aes )
            return security::encrypt_rc4( data, obj_key );
        auto bytes = security::generate_random_bytes( 16 );
        auto iv = utils::to_string( bytes );
        iv.append( security::encrypt_aes_cbc( data, obj_key, iv ) );
        return iv;
    }

    auto encrypt_data_1a( in_data_t data, in_data_t key ) -> data_t
    {
        auto bytes = security::generate_random_bytes( 16 );
        auto iv = utils::to_string( bytes );
        iv.append( security::encrypt_aes_cbc( data, key, iv ) );
        return iv;
    }

    auto decrypt_data_1( in_data_t data, std::shared_ptr<PDFRef> ref, in_data_t key, bool use_aes ) -> data_t
    {
        auto obj_key = get_object_key( ref, key, use_aes );
        obj_key = obj_key.substr( 0, std::min( key.size( ) + 5, (size_t)16 ) );
        if ( !use_aes )
            return security::decrypt_rc4( data, obj_key );
        auto iv = data.substr( 0, 16 );
        auto encrypted = data.substr( 16 );
        return security::decrypt_aes_cbc( encrypted, obj_key, iv );
    }

    auto decrypt_data_1a( in_data_t data, in_data_t key ) -> data_t
    {
        auto iv = data.substr( 0, 16 );
        auto encrypted = data.substr( 16 );
        return security::decrypt_aes_cbc( encrypted, key, iv );
    }

    void pad_or_truncate_password( data_t &out, const std::string &pw, size_t pos = 0 )
    {
        if ( pw.length( ) < 32 )
        {
            std::memcpy( out.data( ) + pos, pw.data( ), pw.size( ) );
            std::memcpy( out.data( ) + pw.size( ) + pos, fill.data( ), 32 - pw.size( ) );
        }
        else
            std::memcpy( out.data( ) + pos, pw.data( ), 32 );
    }

    auto compute_file_encryption_key_2( const std::string &password, const in_data_t &o, std::uint32_t p,
                                        const in_data_t &id, std::int32_t length, std::uint8_t revision,
                                        bool encrypt_metadata ) -> data_t
    {
        const auto ff4 = utils::from_hex_string( "ffffffff" );
        // a, b)
        auto pw = std::string( 68 + id.size( ) + ( !encrypt_metadata * 4 ), '\0' );
        pad_or_truncate_password( pw, password );
        // c)
        std::memcpy( pw.data( ) + 32, o.data( ), o.size( ) );
        // d)p
        pw[ 64 ] = static_cast<uint8_t>( p );
        pw[ 65 ] = static_cast<uint8_t>( p >> 8 );
        pw[ 66 ] = static_cast<uint8_t>( p >> 16 );
        pw[ 67 ] = static_cast<uint8_t>( p >> 24 );
        // e)
        std::memcpy( pw.data( ) + 68, id.data( ), id.size( ) );
        // f)
        if ( !encrypt_metadata )
            std::memcpy( pw.data( ) + 68 + id.size( ), ff4.data( ), 4 );

        // g)
        auto hash = security::md5( pw );
        // h)
        if ( revision >= 3 )
        {
            auto ret = std::string( length / 8, '\0' );
            std::memcpy( ret.data( ), hash.data( ), ret.size( ) );
            for ( int j = 0; j < 50; ++j )
            {
                hash = security::md5( ret );
                std::memcpy( ret.data( ), hash.data( ), ret.size( ) );
            }
            return ret;
        }
        // i)
        else
        {
            auto ret = std::string( 5, '\0' );
            std::memcpy( ret.data( ), hash.data( ), 5 );
            return ret;
        }
    }

    auto retrieve_file_encryption_key_2a( const std::string &password, AuthenticationState state, in_data_t o,
                                          in_data_t u, in_data_t oe, in_data_t ue, std::int32_t p, in_data_t perms,
                                          std::uint8_t revision, bool *out_encrypt_metadata ) -> data_t
    {
        data_t key;
        if ( state == AuthenticationState::Owner )
            key = security::decrypt_aes_cbc( oe, compute_hash_2b( password, o.substr( 40, 8 ), u, revision ),
                                             std::string( 16, '\0' ) );
        else
            key = security::decrypt_aes_cbc( ue, compute_hash_2b( password, u.substr( 40, 8 ), "", revision ),
                                             std::string( 16, '\0' ) );
        auto decrypted_perm = security::decrypt_aes_ecb( perms, key );
        if ( decrypted_perm.at( 9 ) != 'a' || decrypted_perm.at( 10 ) != 'd' || decrypted_perm.at( 11 ) != 'b' )
            throw std::runtime_error( "perms byte 9 to 11 do not match adb" );

        // used to convert the 32-bit int to a array of 8-bit integers
        union PermUnion {
            std::int32_t p;
            std::int8_t pa[ 4 ];
        };

        PermUnion perm = { p };
        if constexpr ( std::endian::native == std::endian::big )
        {
            if ( perm.pa[ 0 ] == static_cast<std::int16_t>( decrypted_perm.at( 0 ) ) &&
                 perm.pa[ 1 ] == static_cast<std::int16_t>( decrypted_perm.at( 1 ) ) &&
                 perm.pa[ 2 ] == static_cast<std::int16_t>( decrypted_perm.at( 2 ) ) )
                throw std::runtime_error( "p does not match decrypted perm byte 0 to 3" );
        }
        else if ( std::endian::native == std::endian::little )
        {

            if ( perm.pa[ 3 ] == static_cast<std::int16_t>( decrypted_perm.at( 0 ) ) &&
                 perm.pa[ 2 ] == static_cast<std::int16_t>( decrypted_perm.at( 1 ) ) &&
                 perm.pa[ 1 ] == static_cast<std::int16_t>( decrypted_perm.at( 2 ) ) )
                throw std::runtime_error( "p does not match decrypted perm byte 0 to 3" );
        }

        *out_encrypt_metadata = decrypted_perm.at( 8 ) == 'T';
        return key;
    }

    thread_local auto k1 = std::string( 64 * ( 127 + 64 + 48 ), '\0' );

    auto compute_hash_2b( const std::string &password, in_data_t salt, in_data_t user_key, std::uint8_t revision )
        -> data_t
    {
        data_t k;
        data_t t;
        data_t e;
        std::memset( k1.data( ), 0, k1.size( ) );
        auto com_key = std::string( 16, '\0' );
        auto iv = std::string( 16, '\0' );
        auto num = std::string( 16, '\0' );

        auto pw_len = password.length( ) > 127 ? 127 : password.length( );
        std::size_t hash_len = 32;
        auto uk_len = user_key.size( );
        std::size_t seq_len = 0;
        std::size_t len = 0;
        int rounds = 0;
        std::uint64_t rem;

        if ( !salt.empty( ) )
        {
            auto h = std::string( password.length( ) + salt.size( ) + uk_len, '\0' );
            memcpy( h.data( ), password.data( ), pw_len );
            memcpy( h.data( ) + pw_len, salt.data( ), salt.size( ) );
            if ( !user_key.empty( ) )
                memcpy( h.data( ) + pw_len + salt.size( ), user_key.data( ), uk_len );
            k = security::sha256( h );
        }
        else
            k = security::sha256( password );

        if ( revision == 5 )
            return k;

        while ( rounds < 64 || rounds < (std::uint8_t)e.at( len - 1 ) + 32 ) // NOLINT
        {
            // a)
            seq_len = pw_len + hash_len + uk_len;
            len = 64 * seq_len;
            memcpy( k1.data( ), password.data( ), pw_len );
            memcpy( k1.data( ) + pw_len, k.data( ), hash_len );
            if ( !user_key.empty( ) )
                memcpy( k1.data( ) + pw_len + +hash_len, user_key.data( ), uk_len );
            for ( size_t i = 1; i < 64; i++ )
                memcpy( k1.data( ) + ( i * seq_len ), k1.data( ), seq_len );
            // b)
            t = std::string( len, '\0' );
            memcpy( com_key.data( ), k.data( ), 16 );
            memcpy( iv.data( ), k.data( ) + 16, 16 );
            memcpy( t.data( ), k1.data( ), len );
            e = security::encrypt_aes_cbc( t, com_key, iv );
            // c)
            memcpy( num.data( ), e.data( ), 16 );
            uint64_t n1 = 0, n2 = 0, n3 = 0;
            // NOLINTBEGIN
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 0 ) ) << 56;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 1 ) ) << 48;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 2 ) ) << 40;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 3 ) ) << 32;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 4 ) ) << 24;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 5 ) ) << 16;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 6 ) ) << 8;
            n1 |= static_cast<uint64_t>( (std::uint8_t)num.at( 7 ) );
            rem = n1 % 3;
            n2 |= static_cast<uint64_t>( (std::uint8_t)num.at( 8 ) ) << 24;
            n2 |= static_cast<uint64_t>( (std::uint8_t)num.at( 9 ) ) << 16;
            n2 |= static_cast<uint64_t>( (std::uint8_t)num.at( 10 ) ) << 8;
            n2 |= static_cast<uint64_t>( (std::uint8_t)num.at( 11 ) );
            rem = ( n2 | rem << 32 ) % 3;
            n3 |= static_cast<uint64_t>( (std::uint8_t)num.at( 12 ) ) << 24;
            n3 |= static_cast<uint64_t>( (std::uint8_t)num.at( 13 ) ) << 16;
            n3 |= static_cast<uint64_t>( (std::uint8_t)num.at( 14 ) ) << 8;
            n3 |= static_cast<uint64_t>( (std::uint8_t)num.at( 15 ) );
            // NOLINTEND
            rem = ( n3 | rem << 32 ) % 3;

            if ( rem == 0 )
                k = security::sha256( e );
            else if ( rem == 1 )
                k = security::sha384( e );
            else
                k = security::sha512( e );
            hash_len = k.size( );
            rounds++;
        }
        auto ret = data_t( 32, '\0' );
        memcpy( ret.data( ), k.data( ), 32 );
        return ret;
    }

    auto compute_o_3( const std::string &owner_password, const std::string &user_password, std::int32_t length,
                      std::uint8_t revision ) -> data_t
    {
        auto n = revision == 2 ? 5 : length / 8;
        // a)
        std::string pw_str = owner_password.empty( ) ? user_password : owner_password;
        auto pw = std::string( 32, '\0' );
        pad_or_truncate_password( pw, pw_str );
        // b)
        auto hash_1 = security::md5( pw );
        // c)
        if ( revision >= 3 )
            for ( int j = 0; j < 50; ++j )
                hash_1 = security::md5( hash_1 );
        // d)
        auto key_com = std::string( n, '\0' );
        std::memcpy( key_com.data( ), hash_1.data( ), n );
        // e)
        pw_str = user_password;
        pad_or_truncate_password( pw, pw_str );
        // f)
        auto encrypted = security::encrypt_rc4( pw, key_com );
        // g)
        auto key = std::string( key_com.size( ), '\0' );
        if ( revision >= 3 )
        {
            for ( int i = 1; i < 20; ++i )
            {
                memcpy( key.data( ), key_com.data( ), key.size( ) );
                for ( size_t j = 0; j < key.size( ); ++j )
                    key[ j ] ^= i;
                encrypted = security::encrypt_rc4( encrypted, key );
            }
        }
        return encrypted;
    }

    auto compute_u_4( in_data_t key ) -> data_t
    {
        return security::encrypt_rc4( fill, key );
    }

    auto compute_u_5( in_data_t key, in_data_t id ) -> data_t
    {
        auto h = std::string( 32 + id.size( ), '\0' );
        std::memcpy( h.data( ), fill.data( ), 32 );
        std::memcpy( h.data( ) + 32, id.data( ), id.size( ) );
        auto pad_hash = security::md5( h );
        // d)
        auto encrypted = security::encrypt_rc4( pad_hash, key );
        // e)
        auto new_key = std::string( key.size( ), '\0' );
        for ( int k = 1; k <= 19; k++ )
        {
            std::memcpy( new_key.data( ), key.data( ), key.size( ) );
            for ( char &j : new_key )
                j ^= k;
            encrypted = security::encrypt_rc4( encrypted, new_key );
        }
        // f)
        auto u = std::string( 32, '\0' );
        std::memcpy( u.data( ), encrypted.data( ), encrypted.size( ) );
        std::memcpy( u.data( ) + 16, fill.data( ), 16 ); // Pad with arbitrary data
        return u;
    }

    auto authenticate_user_password_6( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                       in_data_t id ) -> data_t
    {
        auto len = dict->get<PDFInteger>( "Length" );
        auto u = dict->get<PDFString>( "U" ); // 32 byte
        auto r = dict->get<PDFInteger>( "R" );
        auto o = dict->get<PDFString>( "O" );  // 32 byte
        auto p = dict->get<PDFInteger>( "P" ); // 4 byte
        auto enc_meta = dict->get<PDFBool>( "EncryptMetadata" );

        if ( !( o && p && len && r ) )
            return "";

        data_t u_com;
        auto key = compute_file_encryption_key_2( password, o->get_data( ), p->value, id, len->value, r->value,
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
            log::get_logger( "pdf" )->error( "revision {} is not supported!", r->as_int( ) );
            return "";
        }
        if ( u_com.substr( 0, 16 ) == u->get_data( ).substr( 0, 16 ) )
            return key;
        return "";
    }

    auto authenticate_owner_password_7( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        in_data_t id ) -> data_t
    {
        auto len = dict->get<PDFInteger>( "Length" );
        auto r = dict->get<PDFInteger>( "R" );
        auto o = dict->get<PDFString>( "O" ); // 32 byte

        if ( !( len && r && o ) )
            return "";
        auto n = r->value == 2 ? 5 : len->value / 8;
        // a)
        auto pw = std::string( 32, '\0' );
        pad_or_truncate_password( pw, password );
        auto hash_1 = security::md5( pw );
        if ( r->value >= 3 )
            for ( int j = 0; j < 50; ++j )
                hash_1 = security::md5( hash_1 );
        auto key = std::string( n, '\0' );
        std::memcpy( key.data( ), hash_1.data( ), n );
        // b)
        data_t u;
        if ( r->value == 2 )
            u = security::decrypt_rc4( o->get_data( ), key );
        else
        {
            u = o->get_data( );
            auto com_key = std::string( key.size( ), '\0' );
            for ( int i = 19; i >= 0; i-- )
            {
                memcpy( com_key.data( ), key.data( ), key.size( ) );
                for ( char &j : com_key )
                    j ^= i;
                u = security::decrypt_rc4( u, com_key );
            }
        }
        return authenticate_user_password_6( u, dict, id );
    }

    auto compute_u_ue_8( const std::string &password, in_data_t key, uint8_t revision ) -> std::pair<data_t, data_t>
    {
        auto u = std::string( 48, '\0' );
        data_t ue;
        // a)
        auto user_validation_salt = security::generate_random_bytes( 8 );
        auto user_key_salt = security::generate_random_bytes( 8 );
        {
            auto hash = compute_hash_2b( password, utils::to_string( user_validation_salt ), "", revision );
            // TODO: update
            std::memcpy( u.data( ), hash.data( ), 32 );
            std::memcpy( u.data( ) + 32, user_validation_salt.data( ), 8 );
            std::memcpy( u.data( ) + 40, user_key_salt.data( ), 8 );
        }
        {
            auto hash = compute_hash_2b( password, utils::to_string( user_key_salt ), "", revision );
            ue = security::encrypt_aes_cbc( key, hash, std::string( 16, '\0' ) );
        }

        return { u, ue };
    }

    auto compute_o_oe_9( const std::string &password, in_data_t key, in_data_t u, uint8_t revision )
        -> std::pair<data_t, data_t>
    {
        auto o = std::string( 48, '\0' );
        data_t oe;
        // a)
        auto owner_validation_salt = security::generate_random_bytes( 8 );
        auto owner_key_salt = security::generate_random_bytes( 8 );
        {
            auto hash = compute_hash_2b( password, utils::to_string( owner_validation_salt ), u, revision );
            // TODO: update
            std::memcpy( o.data( ), hash.data( ), 32 );
            std::memcpy( o.data( ) + 32, owner_validation_salt.data( ), 8 );
            std::memcpy( o.data( ) + 40, owner_key_salt.data( ), 8 );
        }
        {
            auto hash = compute_hash_2b( password, utils::to_string( owner_key_salt ), u, revision );
            oe = security::encrypt_aes_cbc( key, hash, std::string( 16, '\0' ) );
        }
        return { o, oe };
    }

    auto encrypt_perms_10( std::int32_t p, bool enc_meta_data, in_data_t key ) -> data_t
    {
        auto perm = std::string( 16, '\0' );
        perm.at( 0 ) = static_cast<uint8_t>( p );
        perm.at( 1 ) = static_cast<uint8_t>( p >> 8 );
        perm.at( 2 ) = static_cast<uint8_t>( p >> 16 );
        perm.at( 3 ) = static_cast<uint8_t>( p >> 24 );
        std::memset( perm.data( ) + 4, 0xff, 4 ); // higher order 32 bit to 1
        perm.at( 8 ) = enc_meta_data ? 'T' : 'F';
        perm.at( 9 ) = 'a';
        perm.at( 10 ) = 'd';
        perm.at( 11 ) = 'b';
        auto rand = security::generate_random_bytes( 4 );
        std::memcpy( perm.data( ) + 12, rand.data( ), 4 );
        return security::encrypt_aes_ecb( perm, key );
    }

    auto authenticate_user_password_11( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        bool *out_encrypt_meta ) -> data_t
    {
        auto u = dict->get<PDFString>( "U" );
        auto o = dict->get<PDFString>( "O" );
        auto ue = dict->get<PDFString>( "UE" );
        auto oe = dict->get<PDFString>( "OE" );
        auto p = dict->get<PDFInteger>( "P" );
        auto perm = dict->get<PDFString>( "Perms" );
        auto r = dict->get<PDFInteger>( "R" );
        if ( !( u && ue && o && oe && p && perm && r ) )
            return "";
        // a)
        auto d = std::string( 8, '\0' );
        memcpy( d.data( ), u->get_data( ).data( ) + 32, 8 );
        auto hash = compute_hash_2b( password, d, "", r->value );
        if ( u->get_data( ).substr( 0, 32 ) != hash )
            return "";
        return retrieve_file_encryption_key_2a( password, AuthenticationState::User, o->get_data( ), u->get_data( ),
                                                oe->get_data( ), ue->get_data( ), p->value, perm->get_data( ), r->value,
                                                out_encrypt_meta );
    }

    auto authenticate_owner_password_12( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                         bool *out_encrypt_meta ) -> data_t
    {
        auto u = dict->get<PDFString>( "U" );
        auto o = dict->get<PDFString>( "O" );
        auto ue = dict->get<PDFString>( "UE" );
        auto oe = dict->get<PDFString>( "OE" );
        auto p = dict->get<PDFInteger>( "P" );
        auto perm = dict->get<PDFString>( "Perms" );
        auto r = dict->get<PDFInteger>( "R" );
        if ( !( u && ue && o && oe && p && perm && r ) )
            return "";
        auto o_str = o->get_data( ).substr( 0, 32 );
        auto hash = compute_hash_2b( password, o->get_data( ).substr( 32, 8 ), u->get_data( ), r->value );
        if ( o_str != hash )
            return "";
        return retrieve_file_encryption_key_2a( password, AuthenticationState::Owner, o->get_data( ), u->get_data( ),
                                                oe->get_data( ), ue->get_data( ), p->value, perm->get_data( ), r->value,
                                                out_encrypt_meta );
    }

    auto prep_password( std::string str ) -> std::string
    {
        if ( str.empty( ) )
            return str;

        UErrorCode err = U_ZERO_ERROR;
        auto profile = usprep_openByType( USPREP_RFC4013_SASLPREP, &err );
        if ( err > U_ZERO_ERROR )
        {
            log::get_logger( "pdf" )->warn( "string_prep_open failed Error: {0}, {1}", 0, u_errorName( err ) );
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
            log::get_logger( "pdf" )->warn( "string_prep failed Error: {0}, {1}", 0, u_errorName( err ) );
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