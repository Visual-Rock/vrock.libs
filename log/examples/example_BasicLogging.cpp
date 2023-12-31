#include "fmt/core.h"
#include <iostream>

#define VROCKLIBS_LOG_USE_THREAD_ID
#define VROCKLIBS_LOG_USE_PROCESS_ID
#include <vrock/log.hpp>
#include <vrock/utils.hpp>
using namespace vrock::utils;

int main( )
{
    // auto label = to_string( vrock::log::LogLevel::Debug );
    // auto tmp = vrock::log::this_execution_context;
    // std::source_location location = std::source_location::current( );
    // fmt::println( "file: {}, method: {}, line: {}, column: {}", location.file_name( ), location.function_name( ),
    //               location.line( ), location.column( ) );
    // fmt::println( "thread: {}, process: {}", tmp.thread_id, tmp.process_id );
    // fmt::println( "level: {}",
    //               vrock::log::includes_level( vrock::log::LogLevel::Critical, vrock::log::LogLevel::Critical ) );
    auto logger = vrock::log::Logger( "EXAMPLE", vrock::log::LogLevel::Info );
    logger.add_sink<vrock::log::ConsoleSink>( );
    logger.log( "test", vrock::log::LogLevel::Debug );
    return 0;
}