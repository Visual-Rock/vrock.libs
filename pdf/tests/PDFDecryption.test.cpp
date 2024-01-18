#include <vrock/pdf/PDFDocument.hpp>

#include <gtest/gtest.h>

#include <iostream>

using namespace vrock::pdf;

TEST( DecryptOwnerOnly, BasicAssertions )
{
    std::vector<std::string> files = { "pdfs/Encrypted/R2_O.pdf", "pdfs/Encrypted/R3_O.pdf",
                                       "pdfs/Encrypted/R4_O.pdf", "pdfs/Encrypted/R4_O_AES.pdf",
                                       "pdfs/Encrypted/R5_O.pdf", "pdfs/Encrypted/R6_O.pdf" };
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
    std::vector<std::string> files = { "pdfs/Encrypted/R2_U.pdf", "pdfs/Encrypted/R3_U.pdf",
                                       "pdfs/Encrypted/R4_U.pdf", "pdfs/Encrypted/R4_U_AES.pdf",
                                       "pdfs/Encrypted/R5_U.pdf", "pdfs/Encrypted/R6_U.pdf" };
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
    std::vector<std::string> files = { "pdfs/Encrypted/R2_U_O.pdf", "pdfs/Encrypted/R3_U_O.pdf",
                                       "pdfs/Encrypted/R4_U_O.pdf", "pdfs/Encrypted/R4_U_O_AES.pdf",
                                       "pdfs/Encrypted/R5_U_O.pdf", "pdfs/Encrypted/R6_U_O.pdf" };
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

TEST( DecryptUeserOewner, BasicAssertions )
{
    auto doc = PDFDocument( u8"pdfs/Encrypted/ueser_oewner.pdf" );
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