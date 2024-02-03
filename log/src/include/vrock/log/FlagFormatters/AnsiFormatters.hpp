#pragma once

#include "FlagFormatter.hpp"

namespace vrock::log
{
    class AnsiColorFormatter : public FlagFormatter
    {
    public:
        AnsiColorFormatter( char color, bool background );

        void format( const Message &msg, buffer_t &buffer ) override;

    private:
        std::string code_;
    };

    class AnsiResetFormatter : public FlagFormatter
    {
    public:
        AnsiResetFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class AnsiLogLevelColorFormatter : public FlagFormatter
    {
    public:
        AnsiLogLevelColorFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class AnsiBoldFormatter : public FlagFormatter
    {
    public:
        AnsiBoldFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class AnsiUnderlineFormatter : public FlagFormatter
    {
    public:
        AnsiUnderlineFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };
} // namespace vrock::log