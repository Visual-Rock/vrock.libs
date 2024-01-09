#include "vrock/log/FlagFormatters/PatternFormatter.hpp"

#include "vrock/log/FlagFormatters/AlignFormatter.hpp"
#include "vrock/log/FlagFormatters/AnsiFormatters.hpp"
#include "vrock/log/FlagFormatters/GeneralFormatters.hpp"
#include "vrock/log/FlagFormatters/SourceFormatters.hpp"
#include "vrock/log/FlagFormatters/TimeFormatters.hpp"

namespace vrock::log
{
    std::string_view global_pattern = "[ %x %T ] [ %n ] %@[ %<5l ]%$ %v";

    auto set_global_pattern( std::string_view pattern ) -> void
    {
        global_pattern = pattern;
    }

    auto get_global_pattern( ) -> std::string_view
    {
        return global_pattern;
    }

    auto compile_pattern( const std::string_view pattern, bool use_color ) -> formatter_collection_t
    {
        formatter_collection_t collection{ };

        char c;
        std::string user_string{ };
        std::unique_ptr<AlignFormatter> align_formatter = nullptr;
        std::unique_ptr<FlagFormatter> formatter = nullptr;
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

                if ( flag == '<' || flag == '^' || flag == '>' )
                {
                    bool truncate = false;
                    std::uint8_t width = pattern[ ++i ] - 48;
                    if ( std::isdigit( pattern[ i + 1 ] ) )
                    {
                        width *= 10;
                        width += pattern[ ++i ] - 48;
                    }
                    if ( pattern[ i + 1 ] == '!' )
                    {
                        ++i;
                        truncate = true;
                    }

                    switch ( flag )
                    {
                    case '<':
                        align_formatter = std::make_unique<LeftAlignFormatter>( width, truncate );
                        break;
                    case '^':
                        align_formatter = std::make_unique<CenterAlignFormatter>( width, truncate );
                        break;
                    case '>':
                        align_formatter = std::make_unique<RightAlignFormatter>( width, truncate );
                        break;
                    }

                    flag = pattern[ ++i ];
                }

                switch ( flag )
                {
                case 'l':
                    formatter = std::make_unique<LevelFormatter>( );
                    break;
                case 'v':
                    formatter = std::make_unique<MessageFormatter>( );
                    break;
                case 't':
                    formatter = std::make_unique<ThreadIDFormatter>( );
                    break;
                case 'n':
                    formatter = std::make_unique<LoggerNameFormatter>( );
                    break;
                case 'P':
                    formatter = std::make_unique<ProcessIDFormatter>( );
                    break;
                    // Ansi
                case 'Q':
                case 'q': {
                    ++i;
                    if ( use_color )
                        formatter = std::make_unique<AnsiColorFormatter>( pattern[ i ], flag == 'Q' );
                    break;
                }
                case '$':
                    if ( use_color )
                        formatter = std::make_unique<AnsiResetFormatter>( );
                    break;
                case '@':
                    if ( use_color )
                        formatter = std::make_unique<AnsiLogLevelColorFormatter>( );
                    break;
                case '_':
                    if ( use_color )
                        formatter = std::make_unique<AnsiUnderlineFormatter>( );
                    break;
                case '*':
                    if ( use_color )
                        formatter = std::make_unique<AnsiBoldFormatter>( );
                    break;
                    // Source
                case 's':
                    formatter = std::make_unique<SourceFileFormatter<false>>( );
                    break;
                case 'g':
                    formatter = std::make_unique<SourceFileFormatter<true>>( );
                    break;
                case '#':
                    formatter = std::make_unique<SourceLineFormatter>( );
                    break;
                case '+':
                    formatter = std::make_unique<SourceColumnFormatter>( );
                    break;
                case '!':
                    formatter = std::make_unique<SourceFunctionFormatter>( );
                    break;
                // Time
                case 'A':
                    formatter = std::make_unique<WeekdayNameFormatter>( );
                    break;
                case 'a':
                    formatter = std::make_unique<WeekdayShortNameFormatter>( );
                    break;
                case 'B':
                    formatter = std::make_unique<MonthNameFormatter>( );
                    break;
                case 'b':
                    formatter = std::make_unique<MonthShortNameFormatter>( );
                    break;
                case 'c':
                    formatter = std::make_unique<DateTimeFormatter>( );
                    break;
                case 'C':
                    formatter = std::make_unique<ShortYearFormatter>( );
                    break;
                case 'Y':
                    formatter = std::make_unique<YearFormatter>( );
                    break;
                case 'D':
                    formatter = std::make_unique<ShortDateFormatter>( );
                    break;
                case 'x':
                    formatter = std::make_unique<LocalDateFormatter>( );
                    break;
                case 'm':
                    formatter = std::make_unique<MonthFormatter>( );
                    break;
                case 'd':
                    formatter = std::make_unique<DayFormatter>( );
                    break;
                case 'H':
                    formatter = std::make_unique<Hour24Formatter>( );
                    break;
                case 'I':
                    formatter = std::make_unique<Hour12Formatter>( );
                    break;
                case 'M':
                    formatter = std::make_unique<MinuteFormatter>( );
                    break;
                case 'S':
                    formatter = std::make_unique<SecondFormatter>( );
                    break;
                case 'e':
                    formatter = std::make_unique<MillisecondFormatter>( );
                    break;
                case 'f':
                    formatter = std::make_unique<MicrosecondFormatter>( );
                    break;
                case 'F':
                    formatter = std::make_unique<NanosecondFormatter>( );
                    break;
                case 'p':
                    formatter = std::make_unique<AmPmFormatter>( );
                    break;
                case 'r':
                    formatter = std::make_unique<Time12Formatter>( );
                    break;
                case 'R':
                    formatter = std::make_unique<Time24Formatter>( );
                    break;
                case 'T':
                    formatter = std::make_unique<ISO8601TimeFormatter>( );
                    break;
                case 'z':
                    formatter = std::make_unique<ISO8601TimezoneFormatter>( );
                    break;
                case 'E':
                    formatter = std::make_unique<TimeSinceEpochFormatter>( );
                    break;
                default:
                    break;
                }

                if ( align_formatter != nullptr )
                {
                    align_formatter->set_formatter( std::move( formatter ) );
                    collection.push_back( std::move( align_formatter ) );
                }
                else
                {
                    if ( formatter != nullptr )
                        collection.push_back( std::move( formatter ) );
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