import vrock.pdf.PDFDocument;
import vrock.pdf.PDFEncryption;

#include <gtest/gtest.h>

#include <iostream>

using namespace vrock::pdf;

TEST( DecryptOwnerOnly, BasicAssertions )
{
    std::vector<std::string> files = { "Encrypted/R2_O.pdf",     "Encrypted/R3_O.pdf", "Encrypted/R4_O.pdf",
                                       "Encrypted/R4_O_AES.pdf", "Encrypted/R5_O.pdf", "Encrypted/R6_O.pdf" };
    for ( const auto &file : files )
    {
        std::cout << "Testing: " << file << std::endl;
        auto doc = PDFDocument( file );
        auto sec = doc.decryption_handler->to<PDFStandardSecurityHandler>( );
        EXPECT_NE( sec, nullptr );
        if ( sec )
        {
            EXPECT_EQ( sec->is_encrypted( ), true );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->has_user_password( ), false );
            EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
            EXPECT_EQ( sec->authenticate( "user" ), AuthenticationState::Failed );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->authenticate( "wrong" ), AuthenticationState::Failed );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->authenticate( "root" ), AuthenticationState::Owner );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
        }
    }
}

TEST( DecryptUserOnly, BasicAssertions )
{
    std::vector<std::string> files = { "Encrypted/R2_U.pdf",     "Encrypted/R3_U.pdf", "Encrypted/R4_U.pdf",
                                       "Encrypted/R4_U_AES.pdf", "Encrypted/R5_U.pdf", "Encrypted/R6_U.pdf" };
    for ( const auto &file : files )
    {
        std::cout << "Testing: " << file << std::endl;
        auto doc = PDFDocument( file );
        auto sec = doc.decryption_handler->to<PDFStandardSecurityHandler>( );
        EXPECT_NE( sec, nullptr );
        if ( sec )
        {
            EXPECT_EQ( sec->is_encrypted( ), true );
            EXPECT_EQ( sec->is_authenticated( ), false );
            EXPECT_EQ( sec->has_user_password( ), true );
            EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
            EXPECT_EQ( sec->authenticate( "user" ), AuthenticationState::Owner );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->authenticate( "wrong" ), AuthenticationState::Failed );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->authenticate( "root" ), AuthenticationState::Failed );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
        }
    }
}

TEST( DecryptUserOwner, BasicAssertions )
{
    std::vector<std::string> files = { "Encrypted/R2_U_O.pdf",     "Encrypted/R3_U_O.pdf", "Encrypted/R4_U_O.pdf",
                                       "Encrypted/R4_U_O_AES.pdf", "Encrypted/R5_U_O.pdf", "Encrypted/R6_U_O.pdf" };
    for ( const auto &file : files )
    {
        std::cout << "Testing: " << file << std::endl;
        auto doc = PDFDocument( file );
        auto sec = doc.decryption_handler->to<PDFStandardSecurityHandler>( );
        EXPECT_NE( sec, nullptr );
        if ( sec )
        {
            EXPECT_EQ( sec->is_encrypted( ), true );
            EXPECT_EQ( sec->is_authenticated( ), false );
            EXPECT_EQ( sec->has_user_password( ), true );
            EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
            EXPECT_EQ( sec->authenticate( "user" ), AuthenticationState::User );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->authenticate( "wrong" ), AuthenticationState::Failed );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->authenticate( "root" ), AuthenticationState::Owner );
            EXPECT_EQ( sec->is_authenticated( ), true );
            EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
        }
    }
}

TEST( DecryptÜserÖwner, BasicAssertions )
{
    auto doc = PDFDocument( "Encrypted/üser_öwner.pdf" );
    auto sec = doc.decryption_handler->to<PDFStandardSecurityHandler>( );
    EXPECT_NE( sec, nullptr );
    if ( sec )
    {
        EXPECT_EQ( sec->is_encrypted( ), true );
        EXPECT_EQ( sec->is_authenticated( ), false );
        EXPECT_EQ( sec->has_user_password( ), true );
        EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
        EXPECT_EQ( sec->authenticate( "üser" ), AuthenticationState::User );
        EXPECT_EQ( sec->is_authenticated( ), true );
        EXPECT_EQ( sec->authenticate( "wröng" ), AuthenticationState::Failed );
        EXPECT_EQ( sec->is_authenticated( ), true );
        EXPECT_EQ( sec->authenticate( "öwner" ), AuthenticationState::Owner );
        EXPECT_EQ( sec->is_authenticated( ), true );
        EXPECT_EQ( sec->has_permission( Permissions::PrintDocument ), true );
    }
}