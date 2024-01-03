#include "vrock/log/FlagFormatters/PatternFormatter.hpp"

#include "vrock/log/FlagFormatters/AnsiFormatters.hpp"
#include "vrock/log/FlagFormatters/GeneralFormatters.hpp"
#include "vrock/log/FlagFormatters/SourceFormatters.hpp"
#include "vrock/log/FlagFormatters/TimeFormatters.hpp"

namespace vrock::log
{
    auto compile_pattern( const std::string_view pattern, bool use_color ) -> formatter_collection_t
    {
        formatter_collection_t collection{ };

        char c;
        std::string user_string{ };
        for ( int i = 0; i < pattern.size( ); ++i )
        {
            c = pattern[ i ];
            // check for formatting flag
            if ( c == '%' )
            {
                if ( i + 1 >= pattern.size( ) )
                    throw std::runtime_error( "invalid formatting string" );

                char flag = pattern[ ++i ];
                if ( flag == '%' )
                {
                    user_string += flag;
                    continue;
                }

                collection.push_back( std::make_unique<UserCharactersFormatter>( user_string ) );
                user_string.clear( );

                switch ( flag )
                {
                case 'l':
                    collection.push_back( std::make_unique<LevelFormatter>( ) );
                    break;
                case 'v':
                    collection.push_back( std::make_unique<MessageFormatter>( ) );
                    break;
                case 't':
                    collection.push_back( std::make_unique<ThreadIDFormatter>( ) );
                    break;
                case 'n':
                    collection.push_back( std::make_unique<LoggerNameFormatter>( ) );
                    break;
                case 'P':
                    collection.push_back( std::make_unique<ProcessIDFormatter>( ) );
                    break;
                // Ansi
                case 'Q':
                case 'q': {
                    ++i;
                    if ( use_color )
                        collection.push_back( std::make_unique<AnsiColorFormatter>( pattern[ i ], flag == 'Q' ) );
                    break;
                }
                case '$':
                    collection.push_back( std::make_unique<AnsiResetFormatter>( ) );
                    break;
                case '^':
                    collection.push_back( std::make_unique<AnsiLogLevelColorFormatter>( ) );
                    break;
                case '_':
                    collection.push_back( std::make_unique<AnsiUnderlineFormatter>( ) );
                    break;
                case '*':
                    collection.push_back( std::make_unique<AnsiBoldFormatter>( ) );
                    break;
                // Source
                case 's':
                    collection.push_back( std::make_unique<SourceFileFormatter<false>>( ) );
                    break;
                case 'g':
                    collection.push_back( std::make_unique<SourceFileFormatter<true>>( ) );
                    break;
                case '#':
                    collection.push_back( std::make_unique<SourceLineFormatter>( ) );
                    break;
                case '@':
                    collection.push_back( std::make_unique<SourceColumnFormatter>( ) );
                    break;
                case '!':
                    collection.push_back( std::make_unique<SourceFunctionFormatter>( ) );
                    break;
                // Time
                case 'A':
                    collection.push_back( std::make_unique<WeekdayNameFormatter>( ) );
                    break;
                case 'a':
                    collection.push_back( std::make_unique<WeekdayShortNameFormatter>( ) );
                    break;
                case 'B':
                    collection.push_back( std::make_unique<MonthNameFormatter>( ) );
                    break;
                case 'b':
                    collection.push_back( std::make_unique<MonthShortNameFormatter>( ) );
                    break;
                case 'c':
                    collection.push_back( std::make_unique<DateTimeFormatter>( ) );
                    break;
                case 'C':
                    collection.push_back( std::make_unique<YearFormatter>( ) );
                    break;
                case 'Y':
                    collection.push_back( std::make_unique<ShortYearFormatter>( ) );
                    break;
                case 'D':
                    collection.push_back( std::make_unique<ShortDateFormatter>( ) );
                    break;
                case 'x':
                    collection.push_back( std::make_unique<LocalDateFormatter>( ) );
                    break;
                default:
                    break;
                }
            }
            else
                user_string += c;
        }

        if ( !user_string.empty( ) )
            collection.push_back( std::make_unique<UserCharactersFormatter>( user_string ) );

        return collection;
    }

} // namespace vrock::log