#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <unistd.h>


#include "Logger.h"

//class Test_Logger: public testing::Test{
//
//};
void SetupLoggerTests(){
    std::filesystem::remove(AppLogger::Logger::GetFilePath());
    sleep(5); // Wait for files to finish being written from previous tests
}

TEST(Logger, HelloWorld){
    SetupLoggerTests();
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);

    AppLogger::Logger::Log("Debug is blue", AppLogger::DEBUG);
    AppLogger::Logger::Log("Info is green", AppLogger::INFO);
    AppLogger::Logger::Log("Warning is yellow", AppLogger::WARNING);
    AppLogger::Logger::Log("Error is red", AppLogger::ERROR);

    AppLogger::Logger::Close(); // make sure stdout and file get flushed before terminating
}

TEST(Logger, HelloWorld2){
    SetupLoggerTests();
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);

    AppLogger::Logger::Log("Debug is blue", AppLogger::DEBUG);
    AppLogger::Logger::Log("Info is green", AppLogger::INFO);
    AppLogger::Logger::Log("Warning is yellow", AppLogger::WARNING);
    AppLogger::Logger::Log("Error is red", AppLogger::ERROR);

    AppLogger::Logger::Close(); // make sure stdout and file get flushed before terminating
}


TEST(Logger, HighFreqFileout){
    SetupLoggerTests();
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);
    AppLogger::Logger::SetStdout(false);

    ulong start_ns = AppLogger::CurrentTime();
    std::cout << "going to log!" << std::endl;
    for (int i=0; i<100000; i++){
        AppLogger::Logger::Log(std::to_string(i), AppLogger::DEBUG);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::INFO);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::WARNING);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::ERROR);
    }
    std::cout << "done logging, going to close logger" << std::endl;
    AppLogger::Logger::Close();
    std::cout << "logger closed" << std::endl;

    ulong end_ns = AppLogger::CurrentTime();

    ulong duration_ms = (end_ns - start_ns) / 1.0e6;
    std::cout << "Wrote 100,000 x4 messages to the log file in " << duration_ms << " ms" << std::endl;

    ASSERT_LE(duration_ms, 2000);

}