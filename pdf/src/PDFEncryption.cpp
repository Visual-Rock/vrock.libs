module;

#include <string>

module vrock.pdf.PDFEncryption;

namespace vrock::pdf
{
    auto PDFStandardSecurityHandler::authenticate( const std::string &pw ) -> AuthenticationState
    {
        return AuthenticationState::Failed;
    }
    auto PDFStandardSecurityHandler::is_authenticated( ) -> bool
    {
        return true;
    }
} // namespace vrock::pdf