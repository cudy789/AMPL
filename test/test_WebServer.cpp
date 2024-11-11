#include <gtest/gtest.h>

#include "common.h"
//#include "WebServer.h"

#include <string>

#include <opencv2/core/core.hpp>

TEST(WebServer, HelloWorld){
    SetupLogger("WebServer_HelloWorld");
    AppLogger::Logger::Log("No test to see here!", AppLogger::SEVERITY::WARNING);

    AppLogger::Logger::Flush();
}