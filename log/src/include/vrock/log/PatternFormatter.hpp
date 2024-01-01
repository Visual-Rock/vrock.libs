#pragma once

#include <string>
#include <utility>

#include "AnsiColors.hpp"
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
            buffer.append( msg.message.data( ), 0, msg.message.length( ) );
        }
    };

    class ThreadIDFormatter : public FlagFormatter
    {
    public:
        ThreadIDFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( msg.execution_context.thread_id, 0, msg.execution_context.thread_id.length( ) );
        }
    };

    class ProcessIDFormatter : public FlagFormatter
    {
    public:
        ProcessIDFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            if ( process_ids_.contains( msg.execution_context.process_id ) )
            {
                buffer.append( process_ids_[ msg.execution_context.process_id ] );
                return;
            }
            process_ids_[ msg.execution_context.process_id ] = std::to_string( msg.execution_context.process_id );
            buffer.append( process_ids_[ msg.execution_context.process_id ] );
        }

    private:
        std::unordered_map<std::size_t, std::string> process_ids_ = { };
    };

    class AnsiColorFormatter : public FlagFormatter
    {
    public:
        AnsiColorFormatter( char color, bool background )
        {
            std::uint8_t c = std::to_underlying( flag_to_color( color ) );
            if ( background )
                c += 10;
            code_ = std::format( "\033[{}m", c );
        }

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( code_ );
        }

    private:
        std::string code_;
    };

    class AnsiResetFormatter : public FlagFormatter
    {
    public:
        AnsiResetFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( "\033[m" );
        }
    };

    class AnsiLogLevelColorFormatter : public FlagFormatter
    {
    public:
        AnsiLogLevelColorFormatter( ) = default;

        void format( const Message &msg, buffer_t &buffer ) override
        {
            buffer.append( get_loglevel_color( msg.level ).command_sequence );
        }
    };

    auto compile_pattern( std::string_view pattern, bool use_color ) -> formatter_collection_t;
} // namespace vrock::log