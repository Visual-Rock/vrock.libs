#pragma once

#include "FlagFormatter.hpp"

#include <unordered_map>

namespace vrock::log
{
    class UserCharactersFormatter : public FlagFormatter
    {
    public:
        explicit UserCharactersFormatter( std::string user_string );

        void format( const Message &msg, buffer_t &buffer ) override;

    private:
        std::string str_;
    };

    class LevelFormatter : public FlagFormatter
    {
    public:
        LevelFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class LoggerNameFormatter : public FlagFormatter
    {
    public:
        LoggerNameFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class MessageFormatter : public FlagFormatter
    {
    public:
        MessageFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class ThreadIDFormatter : public FlagFormatter
    {
    public:
        ThreadIDFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;
    };

    class ProcessIDFormatter : public FlagFormatter
    {
    public:
        ProcessIDFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override;

    private:
        std::unordered_map<std::size_t, std::string> process_ids_ = { };
    };

} // namespace vrock::log