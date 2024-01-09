add_library(vrocklog)
target_include_directories(vrocklog PUBLIC ./include/)
target_sources(vrocklog PRIVATE
        src/AnsiColors.cpp
        src/Logger.cpp

        src/FlagFormatters/AlignFormatter.cpp
        src/FlagFormatters/PatternFormatter.cpp
        src/FlagFormatters/AnsiFormatter.cpp
        src/FlagFormatters/SourceFormatter.cpp
        src/FlagFormatters/TimeFormatters.cpp
        src/FlagFormatters/GeneralFormatter.cpp

        src/Sinks/ConsoleSinks.cpp
        src/Sinks/FileSinks.cpp
        src/Sinks/Sink.cpp
)