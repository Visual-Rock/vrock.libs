#pragma once

#include "FlagFormatter.hpp"

namespace vrock::log
{
    class AlignFormatter : public FlagFormatter
    {
    public:
        AlignFormatter( std::uint8_t width, bool truncate );

        void format( const Message &msg, buffer_t &buffer ) override;

        auto set_formatter( std::unique_ptr<FlagFormatter> _formatter ) -> void;

    protected:
        std::string fmt;
        std::unique_ptr<FlagFormatter> formatter;
        std::uint8_t width;
        bool truncate;
    };

    class LeftAlignFormatter final : public AlignFormatter
    {
    public:
        LeftAlignFormatter( std::uint8_t width, bool truncate );
    };

    class CenterAlignFormatter final : public AlignFormatter
    {
    public:
        CenterAlignFormatter( std::uint8_t width, bool truncate );
    };

    class RightAlignFormatter final : public AlignFormatter
    {
    public:
        RightAlignFormatter( std::uint8_t width, bool truncate );
    };
} // namespace vrock::log