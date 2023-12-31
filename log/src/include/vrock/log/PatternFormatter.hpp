#pragma once

#include <string>
#include <utility>

#include "Message.hpp"

namespace vrock::log
{
    using buffer_t = std::string;

    class FlagFormatter
    {
    public:
        FlagFormatter( ) = default;
        virtual ~FlagFormatter( ) = default;

        virtual void format( const Message &msg, buffer_t buffer ) = 0;
    };

    using formatter_collection_t = std::vector<std::unique_ptr<FlagFormatter>>;

    class UserCharactersFormatter : public FlagFormatter
    {
    public:
        explicit UserCharactersFormatter( std::string user_string ) : str_( std::move( user_string ) )
        {
        }

        void format( const Message &msg, buffer_t buffer ) override
        {
            buffer.append( str_.data( ), buffer.size( ) + str_.data( ) );
        }

    private:
        std::string str_;
    };

    class LevelFormatter : public FlagFormatter
    {
    public:
        LevelFormatter( ) = default;

        void format( const Message &msg, buffer_t buffer ) override
        {
            const auto &lvl = to_string( msg.level );
            buffer.append( lvl.data( ), buffer.size( ) + lvl.data( ) );
        }
    };

    auto compile_pattern( std::string_view pattern ) -> formatter_collection_t;
} // namespace vrock::log