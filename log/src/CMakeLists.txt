add_library(vrocklog)
target_include_directories(vrocklog PUBLIC ./include/)
target_sources(vrocklog PRIVATE
        src/AnsiColors.cpp
        src/LoggerStorage.cpp

        src/FlagFormatters/AlignFormatters.cpp
        src/FlagFormatters/PatternFormatters.cpp
        src/FlagFormatters/AnsiFormatters.cpp
        src/FlagFormatters/SourceFormatters.cpp
        src/FlagFormatters/TimeFormatters.cpp
        src/FlagFormatters/GeneralFormatters.cpp

        src/Sinks/ConsoleSinks.cpp
        src/Sinks/FileSinks.cpp
        src/Sinks/Sink.cpp
)