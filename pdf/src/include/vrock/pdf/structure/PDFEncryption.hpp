#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "PDFObjects.hpp"

#include <vrock/utils/SpanHelpers.hpp>

namespace vrock::pdf
{
    enum class SecurityHandlerType
    {
        Null,
        Standard
    };

    template <typename T>
    auto to_handler_type( ) -> SecurityHandlerType
    {
        static_assert( false, "unknown handler type" );
    }

    enum class Permissions : uint32_t
    {
        PrintDocument = 4 | 4294963904,
        ModifyContent = 8 | 4294963904,
        ExtractData = 16 | 4294963904,
        AddAnnotations = 32 | 4294963904,
        FillForms = 256 | 4294963904,
        AssembleDocument = 1024 | 4294963904,
        PrintFull = 2048 | 4294963904,
    };

    enum class AuthenticationState
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

    class PDFBaseSecurityHandler : public std::enable_shared_from_this<PDFBaseSecurityHandler>
    {
    public:
        PDFBaseSecurityHandler( SecurityHandlerType t ) : type( t )
        {
        }

        virtual auto is_encrypted( ) -> bool = 0;
        virtual auto decrypt( in_data_t, std::shared_ptr<PDFRef> ) -> data_t = 0;
        virtual auto encrypt( in_data_t, std::shared_ptr<PDFRef> ) -> data_t = 0;
        virtual auto has_permission( Permissions ) -> bool = 0;

        OPTNONE_START auto is( SecurityHandlerType t ) -> bool
        {
            if ( this == nullptr )
                return false;
            return t == type;
        }
        OPTNONE_END

        template <class T>
            requires std::is_base_of_v<PDFBaseSecurityHandler, T>
        OPTNONE_START std::shared_ptr<T> as( )
        {
            if ( this == nullptr )
                return nullptr;
            return std::static_pointer_cast<T>( shared_from_this( ) );
        }
        OPTNONE_END

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

    class PDFNullSecurityHandler : public PDFBaseSecurityHandler
    {
    public:
        PDFNullSecurityHandler( );

        auto is_encrypted( ) -> bool final;

        auto decrypt( in_data_t data, std::shared_ptr<PDFRef> ) -> data_t final;

        auto encrypt( in_data_t data, std::shared_ptr<PDFRef> ) -> data_t final;

        auto has_permission( Permissions permissions ) -> bool override;
    };

    class PDFStandardSecurityHandler : public PDFBaseSecurityHandler
    {
    public:
        PDFStandardSecurityHandler( std::shared_ptr<PDFDictionary> encryption_dict, std::shared_ptr<PDFContext> ctx,
                                    std::function<void( )> );

        auto is_encrypted( ) -> bool final;

        auto decrypt( in_data_t data, std::shared_ptr<PDFRef> ) -> data_t final;

        auto encrypt( in_data_t data, std::shared_ptr<PDFRef> ) -> data_t final;

        auto has_user_password( ) -> bool;

        auto authenticate( const std::string &pw ) -> AuthenticationState;

        auto is_authenticated( ) -> bool;

        auto has_permission( Permissions perm ) -> bool override;

    private:
        std::shared_ptr<PDFContext> context;

        std::shared_ptr<PDFDictionary> dict;
        data_t key;
        AuthenticationState state = AuthenticationState::Failed;
        std::uint32_t permissions;
        std::uint8_t revision;
        bool encrypt_metadata = false;
        bool use_aes = false;
        std::function<void( )> fn;
    };

    /// PDF Algorithm declaration
    auto encrypt_data_1( in_data_t data, const std::shared_ptr<PDFRef> &ref, in_data_t key, bool use_aes = true )
        -> data_t;

    auto encrypt_data_1a( in_data_t data, in_data_t key ) -> data_t;

    auto decrypt_data_1( in_data_t data, std::shared_ptr<PDFRef> ref, in_data_t key, bool use_aes = true ) -> data_t;

    auto decrypt_data_1a( in_data_t data, in_data_t key ) -> data_t;

    const auto fill =
        utils::from_hex_string<data_t>( "28BF4E5E4E758A4164004E56FFFA01082E2E00B6D0683E802F0CA9FE6453697A" );

    // auto compute_file_encryption_key_2( const std::string &password, in_data_t o, uint32_t p, in_data_t id,
    //                                     int32_t length, uint8_t revision, bool encrypt_metadata ) -> data_t;

    auto retrieve_file_encryption_key_2a( const std::string &password, AuthenticationState state, in_data_t o,
                                          in_data_t u, in_data_t oe, in_data_t ue, int32_t p, in_data_t perms,
                                          uint8_t revision, bool *out_encrypt_metadata ) -> data_t;

    auto compute_hash_2b( const std::string &password, in_data_t salt, in_data_t user_key, uint8_t revision ) -> data_t;

    auto compute_o_3( const std::string &owner_password, const std::string &user_password, int32_t length,
                      uint8_t revision ) -> data_t;

    auto compute_u_4( in_data_t key ) -> data_t;

    auto compute_u_5( in_data_t key, in_data_t id ) -> data_t;

    auto authenticate_user_password_6( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                       in_data_t id ) -> data_t;

    auto authenticate_owner_password_7( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        in_data_t id ) -> data_t;

    auto compute_u_ue_8( const std::string &password, in_data_t key, uint8_t revision ) -> std::pair<data_t, data_t>;

    auto compute_o_oe_9( const std::string &password, in_data_t key, in_data_t u, uint8_t revision )
        -> std::pair<data_t, data_t>;

    auto encrypt_perms_10( int32_t p, bool enc_meta_data, in_data_t key ) -> data_t;

    auto authenticate_user_password_11( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                        bool *out_encrypt_meta ) -> data_t;

    auto authenticate_owner_password_12( const std::string &password, const std::shared_ptr<PDFDictionary> &dict,
                                         bool *out_encrypt_meta ) -> data_t;

    template <>
    auto to_handler_type<PDFNullSecurityHandler>( ) -> SecurityHandlerType;
    template <>
    auto to_handler_type<PDFStandardSecurityHandler>( ) -> SecurityHandlerType;
} // namespace vrock::pdf