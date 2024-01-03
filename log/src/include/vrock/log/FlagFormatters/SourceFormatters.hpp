#pragma once

#include "FlagFormatter.hpp"

#include <filesystem>

namespace vrock::log
{
    template <bool FullPath>
    class SourceFileFormatter : public FlagFormatter
    {
    public:
        SourceFileFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class SourceLineFormatter : public FlagFormatter
    {
    public:
        SourceLineFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class SourceColumnFormatter : public FlagFormatter
    {
    public:
        SourceColumnFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class SourceFunctionFormatter : public FlagFormatter
    {
    public:
        SourceFunctionFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

} // namespace vrock::log