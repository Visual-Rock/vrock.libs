module;

import vrock.pdf.BaseParser;
export import vrock.pdf.PDFBaseObjects;
import vrock.pdf.PDFEncryption;

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

export module vrock.pdf.PDFObjectParser;

namespace vrock::pdf
{
    export class PDFObjectParser : public BaseParser
    {
    public:
        PDFObjectParser( );
        explicit PDFObjectParser( std::string s );

        auto parse_object( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFBaseObject>; // TODO: Test
        auto parse_dictionary( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFDictionary>;
        auto parse_name( ) -> std::shared_ptr<PDFName>;
        auto parse_hex_string( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFByteString>;
        auto parse_string( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFString>;
        auto parse_array( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFArray>;
        auto parse_number_or_reference( ) -> std::shared_ptr<PDFBaseObject>;
        auto parse_dictionary_or_stream( std::shared_ptr<PDFRef> ref, bool decrypt ) -> std::shared_ptr<PDFBaseObject>;

        // auto parse_version( ) -> vrock::pdf::objects::PDFVersion;

        auto parse_xref( std::size_t offset = -1 ) -> std::vector<std::shared_ptr<XRefTable>>;

        auto set_decryption_handler( std::shared_ptr<PDFBaseSecurityHandler> handler ) -> void
        {
            decryption_handler = std::move( handler );
        }
        auto set_context( std::shared_ptr<PDFContext> ctx ) -> void
        {
            context = std::move( ctx );
        }

    protected:
        const std::unordered_map<char, std::string> string_literal_lookup = {
            { 'n', "\n" }, { 'r', "\r" }, { 't', "\t" },  { 'b', "\b" }, { 'f', "\f" },
            { '(', "(" },  { ')', ")" },  { '\\', "\\" }, { '\n', "" }
        };

        std::shared_ptr<PDFContext> context;
        std::shared_ptr<PDFBaseSecurityHandler> decryption_handler = std::make_shared<PDFNullSecurityHandler>( );

        auto find_end_of_stream( ) -> std::size_t;
    };

    export class PDFParserException : public std::exception
    {
    public:
        explicit PDFParserException( std::string m ) : message( std::move( m ) )
        {
        }

        [[nodiscard]] const char *what( ) const noexcept override
        {
            return message.c_str( );
        }

    private:
        std::string message;
    };
} // namespace vrock::pdf