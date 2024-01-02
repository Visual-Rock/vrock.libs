#pragma once

#include <cstdint>
#include <exception>
#include <format>

#include "LogLevel.hpp"

namespace vrock::log
{
    /**
     * @enum AnsiColor
     * @brief Enumeration representing ANSI color codes.
     */
    enum class AnsiColor : std::uint8_t
    {
        Black = 30,   ///< Black color code.
        Red = 31,     ///< Red color code.
        Green = 32,   ///< Green color code.
        Yellow = 33,  ///< Yellow color code.
        Blue = 34,    ///< Blue color code.
        Magenta = 35, ///< Magenta color code.
        Cyan = 36,    ///< Cyan color code.
        White = 37,   ///< White color code.
        Default = 39  ///< Default color code.
    };

    /**
     * @brief Converts a flag to the corresponding ANSI color.
     * @param f The input flag representing a color ('k', 'r', 'g', 'y', 'b', 'm', 'c', 'w', 'd').
     * @return The ANSI color corresponding to the input flag.
     * @throws std::runtime_error if the input flag is not recognized or supported.
     *
     * Converts a flag to the corresponding ANSI color. Valid flags are:
     * - 'k': Black
     * - 'r': Red
     * - 'g': Green
     * - 'y': Yellow
     * - 'b': Blue
     * - 'm': Magenta
     * - 'c': Cyan
     * - 'w': White
     * - 'd': Default
     *
     * Example usage:
     * @code
     * char flag = 'r';
     * AnsiColor color = flag_to_color(flag);
     * @endcode
     *
     * @note This function throws an exception if the input flag is not recognized or supported.
     */
    auto flag_to_color( char f ) -> AnsiColor;

    class LogLevelColor
    {
    public:
        constexpr explicit LogLevelColor( AnsiColor color, bool bold = false, bool underlined = false,
                                          AnsiColor background = AnsiColor::Default )
            : color( color ), bold( bold ), underlined( underlined ), background( background )
        {
            if ( background != AnsiColor::Default )
                command_sequence.append(
                    std::format( "\033[{}m", static_cast<std::underlying_type_t<AnsiColor>>( background ) + 10 ) );
            if ( color != AnsiColor::Default )
                command_sequence.append(
                    std::format( "\033[{}m", static_cast<std::underlying_type_t<AnsiColor>>( color ) ) );
            if ( bold )
                command_sequence.append( "\033[1m" );
            if ( underlined )
                command_sequence.append( "\033[4m" );
        }

        AnsiColor color;
        bool bold, underlined;
        AnsiColor background;

        std::string command_sequence;
    };

    inline LogLevelColor TraceColor = LogLevelColor( AnsiColor::White, true );
    inline LogLevelColor DebugColor = LogLevelColor( AnsiColor::Cyan, true );
    inline LogLevelColor InfoColor = LogLevelColor( AnsiColor::Green, true );
    inline LogLevelColor WarnColor = LogLevelColor( AnsiColor::Yellow, true );
    inline LogLevelColor ErrorColor = LogLevelColor( AnsiColor::Red, true );
    inline LogLevelColor CriticalColor = LogLevelColor( AnsiColor::White, true, false, AnsiColor::Red );

    auto get_loglevel_color( const LogLevel &level ) -> const LogLevelColor &;
} // namespace vrock::log