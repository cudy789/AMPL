#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <unistd.h>


#include "Logger.h"

void SetupLoggerTests(){
    std::filesystem::remove(AppLogger::Logger::GetFilePath());
}

//TEST(Logger, HelloWorld){
//    SetupLoggerTests();
//    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);
//
//    AppLogger::Logger::Log("Debug is blue", AppLogger::DEBUG);
//    AppLogger::Logger::Log("Info is green", AppLogger::INFO);
//    AppLogger::Logger::Log("Warning is yellow", AppLogger::WARNING);
//    AppLogger::Logger::Log("Error is red", AppLogger::ERROR);
//
//    AppLogger::Logger::Close(); // make sure stdout and file get flushed before terminating
//}
//
//TEST(Logger, HelloWorld2){
//    SetupLoggerTests();
//    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);
//
//    AppLogger::Logger::Log("Debug is blue", AppLogger::DEBUG);
//    AppLogger::Logger::Log("Info is green", AppLogger::INFO);
//    AppLogger::Logger::Log("Warning is yellow", AppLogger::WARNING);
//    AppLogger::Logger::Log("Error is red", AppLogger::ERROR);
//
//    AppLogger::Logger::Close(); // make sure stdout and file get flushed before terminating
//}


TEST(Logger, HighFreqFileout){
    SetupLoggerTests();
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);
    AppLogger::Logger::SetStdout(false);

    std::cout << "going to log!" << std::endl;
    ulong start_ns = AppLogger::CurrentTime();
    for (int i=0; i<100000; i++){
        AppLogger::Logger::Log(std::to_string(i), AppLogger::DEBUG);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::INFO);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::WARNING);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::ERROR);
    }
    ulong end_ns = AppLogger::CurrentTime();
    ulong duration_ms = (end_ns - start_ns) / 1.0e6;
    std::cout << "enqueueing 400,000 log messages took " << duration_ms << " ms, on average " << duration_ms / (400000.0) << " ms per message" << std::endl;
    std::cout << "closing logger..." << std::endl;
    AppLogger::Logger::Close();
    std::cout << "logger closed" << std::endl;

    ulong end_close_ns = AppLogger::CurrentTime();
    ulong full_duration_ms = (end_close_ns - start_ns) / 1.0e6;


    std::cout << "Wrote 100,000 x4 messages to the log file in " << full_duration_ms << " ms" << std::endl;

    ASSERT_LE(duration_ms, 500);
    ASSERT_LE(full_duration_ms, 2000);

}