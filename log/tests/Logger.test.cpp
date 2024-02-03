#include <gtest/gtest.h>

#include "vrock/log.hpp"

using namespace vrock::log;

class TestSink : public Sink
{
public:
    TestSink( std::string_view pattern = "" ) : Sink( pattern, false )
    {
    }
    ~TestSink( ) override = default;

    void log( const Message &message ) override
    {
        message_ = write( message );
    }

    void flush( ) override
    {
    }

    std::string message_;
};

TEST( LoggerTest, LoggerTest )
{
    auto logger = make_logger( "TEST", LogLevel::Info, false, "[ %l ] %v" );
    auto sink = std::make_shared<TestSink>( );
    logger->add_sink( sink );

    logger->info( "Hello, World! {}", 3 );
    EXPECT_EQ( sink->message_, "[ info ] Hello, World! 3" );
}