module;

import vrock.pdf.PDFBaseObjects;
import vrock.utils.ByteArray;

#include <cstdint>
#include <functional>
#include <memory>

export module vrock.pdf.PDFEncryption;

namespace vrock::pdf
{
    class PDFDocument;

    export enum SecurityHandlerType
    {
        Null,
        Standard
    };

    template <typename T>
    auto to_handler_type( ) -> SecurityHandlerType
    {
        static_assert( true, "unknown handler type" );
    }

    export enum class Permissions : uint32_t
    {
        PrintDocument = 4 | 4294963904,
        ModifyContent = 8 | 4294963904,
        ExtractData = 16 | 4294963904,
        AddAnnotations = 32 | 4294963904,
        FillForms = 256 | 4294963904,
        AssembleDocument = 1024 | 4294963904,
        PrintFull = 2048 | 4294963904,
    };

    export enum class AuthenticationState
    {
        Failed = -1,
        User = 0,
        Owner = 1
    };

    class PDFEncryptedException : std::exception
    {
    public:
        explicit PDFEncryptedException( std::string message ) : msg( std::move( message ) )
        {
        }

        [[nodiscard]] const char *what( ) const noexcept override
        {
            return msg.c_str( );
        }

    private:
        std::string msg;
    };

    export class PDFBaseSecurityHandler : public std::enable_shared_from_this<PDFBaseSecurityHandler>
    {
    public:
        PDFBaseSecurityHandler( SecurityHandlerType t ) : type( t )
        {
        }

        virtual auto is_encrypted( ) -> bool = 0;
        virtual auto decrypt( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFRef> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;
        virtual auto encrypt( std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<PDFRef> )
            -> std::shared_ptr<utils::ByteArray<>> = 0;
        virtual auto has_permission( Permissions ) -> bool = 0;

        auto is( SecurityHandlerType t ) -> bool
        {
            if ( this == nullptr )
                return false;
            return t == type;
        }

        template <class T>
            requires std::is_base_of_v<PDFBaseSecurityHandler, T>
        std::shared_ptr<T> as( )
        {
            if ( this == nullptr )
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
        }

        template <class T>
            requires std::is_base_of_v<PDFBaseSecurityHandler, T>
        auto to( ) -> std::shared_ptr<T>
        {
            if ( is( to_handler_type<T>( ) ) )
                return as<T>( );
            return nullptr;
        }

        const SecurityHandlerType type;
    };

    export class PDFNullSecurityHandler : public PDFBaseSecurityHandler
    {
    public:
        PDFNullSecurityHandler( ) : PDFBaseSecurityHandler( SecurityHandlerType::Null )
        {
        }

        auto is_encrypted( ) -> bool final
        {
            return false;
        }

        auto decrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> )
            -> std::shared_ptr<utils::ByteArray<>> final
        {
            return data;
        }

        auto encrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> )
            -> std::shared_ptr<utils::ByteArray<>> final
        {
            return data;
        }

        auto has_permission( Permissions permissions ) -> bool override
        {
            return true;
        }
    };

    export class PDFStandardSecurityHandler : public PDFBaseSecurityHandler
    {
    public:
        PDFStandardSecurityHandler( std::shared_ptr<PDFDictionary> encryption_dict, std::shared_ptr<PDFContext> ctx,
                                    std::function<void( )> );

        auto is_encrypted( ) -> bool final
        {
            return true;
        }

        auto decrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto encrypt( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> )
            -> std::shared_ptr<utils::ByteArray<>> final;

        auto has_user_password( ) -> bool
        {
            return authenticate( "" ) == AuthenticationState::Failed;
        }

        auto authenticate( const std::string &pw ) -> AuthenticationState;

        auto is_authenticated( ) -> bool
        {
            return state != AuthenticationState::Failed;
        }

        auto has_permission( Permissions perm ) -> bool
        {
            return ( permissions & (std::int32_t)perm ) || state == AuthenticationState::Owner;
        }

    private:
        std::shared_ptr<PDFContext> context;

        std::shared_ptr<PDFDictionary> dict;
        std::shared_ptr<utils::ByteArray<>> key;
        AuthenticationState state = AuthenticationState::Failed;
        std::uint32_t permissions;
        std::uint8_t revision;
        bool encrypt_metadata = false;
        bool use_aes = false;
        std::function<void( )> fn;
    };

    /// PDF Algorithm declaration
    auto encrypt_data_1( std::shared_ptr<utils::ByteArray<>> data, const std::shared_ptr<PDFRef> &ref,
                         std::shared_ptr<utils::ByteArray<>> key, bool use_aes = true )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto encrypt_data_1a( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<utils::ByteArray<>> key )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto decrypt_data_1( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<PDFRef> ref,
                         std::shared_ptr<utils::ByteArray<>> key, bool use_aes = true )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto decrypt_data_1a( std::shared_ptr<utils::ByteArray<>> data, std::shared_ptr<utils::ByteArray<>> key )
        -> std::shared_ptr<utils::ByteArray<>>;

    const auto fill =
        utils::from_hex_string_shared( "28BF4E5E4E758A4164004E56FFFA01082E2E00B6D0683E802F0CA9FE6453697A" );

    auto compute_file_encryption_key_2( const std::string &password, const std::shared_ptr<utils::ByteArray<>> &o,
                                        uint32_t p, const std::shared_ptr<utils::ByteArray<>> &id, int32_t length,
                                        uint8_t revision, bool encrypt_metadata )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto retrieve_file_encryption_key_2a( const std::string &password, AuthenticationState state,
                                          std::shared_ptr<utils::ByteArray<>> o, std::shared_ptr<utils::ByteArray<>> u,
                                          std::shared_ptr<utils::ByteArray<>> oe,
                                          std::shared_ptr<utils::ByteArray<>> ue, int32_t p,
                                          std::shared_ptr<utils::ByteArray<>> perms, uint8_t revision,
                                          bool *out_encrypt_metadata ) -> std::shared_ptr<utils::ByteArray<>>;

    auto compute_hash_2b( const std::string &password, std::shared_ptr<utils::ByteArray<>> salt,
                          std::shared_ptr<utils::ByteArray<>> user_key, uint8_t revision )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto compute_o_3( const std::string &owner_password, const std::string &user_password, int32_t length,
                      uint8_t revision ) -> std::shared_ptr<utils::ByteArray<>>;

    auto compute_u_4( std::shared_ptr<utils::ByteArray<>> key ) -> std::shared_ptr<utils::ByteArray<>>;

    auto compute_u_5( std::shared_ptr<utils::ByteArray<>> key, std::shared_ptr<utils::ByteArray<>> id )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto authenticate_user_password_6( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                       std::shared_ptr<utils::ByteArray<>> id ) -> std::shared_ptr<utils::ByteArray<>>;

    auto authenticate_owner_password_7( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        std::shared_ptr<utils::ByteArray<>> id ) -> std::shared_ptr<utils::ByteArray<>>;

    auto compute_u_ue_8( const std::string &password, std::shared_ptr<utils::ByteArray<>> key, uint8_t revision )
        -> std::pair<std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<utils::ByteArray<>>>;

    auto compute_o_oe_9( const std::string &password, std::shared_ptr<utils::ByteArray<>> key,
                         std::shared_ptr<utils::ByteArray<>> u, uint8_t revision )
        -> std::pair<std::shared_ptr<utils::ByteArray<>>, std::shared_ptr<utils::ByteArray<>>>;

    auto encrypt_perms_10( int32_t p, bool enc_meta_data, std::shared_ptr<utils::ByteArray<>> key )
        -> std::shared_ptr<utils::ByteArray<>>;

    auto authenticate_user_password_11( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        bool *out_encrypt_meta ) -> std::shared_ptr<utils::ByteArray<>>;

    auto authenticate_owner_password_12( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                         bool *out_encrypt_meta ) -> std::shared_ptr<utils::ByteArray<>>;

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