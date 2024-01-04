#include "vrock/log/FlagFormatters/AlignFormatter.hpp"

namespace vrock::log
{
    AlignFormatter::AlignFormatter( std::uint8_t width, bool truncate )
        : formatter( nullptr ), width( width ), truncate( truncate )
    {
    }

    void AlignFormatter::format( const Message &msg, buffer_t &buffer )
    {
        std::string formatted;
        formatter->format( msg, formatted );
        if ( formatted.size( ) >= width && truncate )
        {
            buffer.append( formatted.substr( 0, width ) );
        }
        else
        {
            buffer.append( std::vformat( fmt, std::make_format_args( formatted ) ) );
        }
    }

    auto AlignFormatter::set_formatter( std::unique_ptr<FlagFormatter> _formatter ) -> void
    {
        formatter = std::move( _formatter );
    }

    LeftAlignFormatter::LeftAlignFormatter( std::uint8_t width, bool truncate ) : AlignFormatter( width, truncate )
    {
        fmt = std::format( "{{0:<{0}}}", width );
    }

    CenterAlignFormatter::CenterAlignFormatter( std::uint8_t width, bool truncate ) : AlignFormatter( width, truncate )
    {
        fmt = std::format( "{{0:^{0}}}", width );
    }

    RightAlignFormatter::RightAlignFormatter( std::uint8_t width, bool truncate ) : AlignFormatter( width, truncate )
    {
        fmt = std::format( "{{0:>{0}}}", width );
    }
} // namespace vrock::log