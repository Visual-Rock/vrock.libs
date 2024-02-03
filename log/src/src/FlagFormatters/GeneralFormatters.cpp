#include "vrock/log/FlagFormatters/GeneralFormatters.hpp"

namespace vrock::log
{
    UserCharactersFormatter::UserCharactersFormatter( std::string user_string ) : str_( std::move( user_string ) )
    {
    }

    void UserCharactersFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( str_ );
    }

    void LevelFormatter::format( const Message &msg, buffer_t &buffer )
    {
        const auto &lvl = to_string( msg.level );
        buffer.append( lvl.data( ) );
    }

    void LoggerNameFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( msg.logger_name );
    }

    void MessageFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( msg.message.data( ), 0, msg.message.length( ) );
    }

    void ThreadIDFormatter::format( const Message &msg, buffer_t &buffer )
    {
        buffer.append( msg.execution_context.thread_id, 0, msg.execution_context.thread_id.length( ) );
    }
    
    void ProcessIDFormatter::format( const Message &msg, buffer_t &buffer )
    {
        if ( process_ids_.contains( msg.execution_context.process_id ) )
        {
            buffer.append( process_ids_[ msg.execution_context.process_id ] );
            return;
        }
        process_ids_[ msg.execution_context.process_id ] = std::to_string( msg.execution_context.process_id );
        buffer.append( process_ids_[ msg.execution_context.process_id ] );
    }
} // namespace vrock::log