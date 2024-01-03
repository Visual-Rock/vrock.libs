#include "vrock/log/FlagFormatters/SourceFormatters.hpp"

namespace vrock::log
{
    template <>
    void SourceFileFormatter<true>::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( msg.source_location.file_name( ) );
    }

    template <>
    void SourceFileFormatter<false>::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::filesystem::path( msg.source_location.file_name( ) ).filename( ) );
    }

    void SourceLineFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::to_string( msg.source_location.line( ) ) );
    }

    void SourceColumnFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( std::to_string( msg.source_location.column( ) ) );
    }
    
    void SourceFunctionFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( msg.source_location.function_name( ) );
    }
} // namespace vrock::log