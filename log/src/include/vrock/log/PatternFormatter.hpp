#pragma once

#include <string>
#include <utility>

#include "Message.hpp"

#include <unordered_map>

namespace vrock::log
{
    using buffer_t = std::string;

    class FlagFormatter
    {
    public:
        FlagFormatter( ) = default;
        virtual ~FlagFormatter( ) = default;

        virtual void format( const Message &msg, buffer_t &buffer ) = 0;
    };

    using formatter_collection_t = std::vector<std::unique_ptr<FlagFormatter>>;

    class UserCharactersFormatter : public FlagFormatter
    {
    public:
        explicit UserCharactersFormatter( std::string user_string ) : str_( std::move( user_string ) )
        {
        }

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( str_ );
        }

    private:
        std::string str_;
    };

    class LevelFormatter : public FlagFormatter
    {
    public:
        LevelFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            const auto &lvl = to_string( msg.level );
            buffer.append( lvl.data( ) );
        }
    };

    class LoggerNameFormatter : public FlagFormatter
    {
    public:
        LoggerNameFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( msg.logger_name );
        }
    };

    class MessageFormatter : public FlagFormatter
    {
    public:
        MessageFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( msg.message.data( ) );
        }
    };

    class ThreadIDFormatter : public FlagFormatter
    {
    public:
        ThreadIDFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( msg.execution_context.thread_id );
        }

    private:
        std::unordered_map<std::size_t, std::string> ids_;
    };

    auto compile_pattern( std::string_view pattern ) -> formatter_collection_t;
} // namespace vrock::log