#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <utility>

#include "Logger.h"
#include "TimeUtils.h"

void SetupLoggerTests(std::string log_path){
    std::filesystem::remove(log_path);
    AppLogger::Logger::SetFilepath(std::move(log_path));
    AppLogger::Logger::SetStdout(true);
    AppLogger::Logger::SetFileout(true);
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);
}

void LogColors(){
    AppLogger::Logger::Log("Debug is blue", AppLogger::DEBUG);
    AppLogger::Logger::Log("Info is green", AppLogger::INFO);
    AppLogger::Logger::Log("Warning is yellow", AppLogger::WARNING);
    AppLogger::Logger::Log("Error is red", AppLogger::ERROR);
}

TEST(Logger, HelloWorld){
    std::string logfile = "./test_logger_HelloWorld.txt";
    SetupLoggerTests(logfile);

    LogColors();

    AppLogger::Logger::Flush(); // make sure stdout and file get flushed before terminating
    ASSERT_TRUE(std::filesystem::exists(logfile));
}

TEST(Logger, HelloWorldStdOut){
    std::string logfile = "./test_logger_HelloWorldStdOut.txt";
    SetupLoggerTests(logfile);

    AppLogger::Logger::SetFileout(false);

    LogColors();

    AppLogger::Logger::Flush(); // make sure stdout and file get flushed before terminating
    ASSERT_FALSE(std::filesystem::exists(logfile));
}

TEST(Logger, HelloWorldFileout){
    std::string logfile = "./test_logger_HelloWorldFileout.txt";
    SetupLoggerTests(logfile);
    AppLogger::Logger::SetStdout(false);

    LogColors();

    AppLogger::Logger::Flush(); // make sure stdout and file get flushed before terminating
    ASSERT_TRUE(std::filesystem::exists(logfile));
}


TEST(Logger, HighFreqFileout){
    std::string logfile = "./test_logger_HighFreqFileout.txt";
    SetupLoggerTests(logfile);
    AppLogger::Logger::SetStdout(false);

    int n_messages = 100000;

    std::cout << "going to log!" << std::endl;
    ulong start_ns = CurrentTime();
    for (int i=0; i<n_messages; i++){
        AppLogger::Logger::Log(std::to_string(i), AppLogger::DEBUG);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::INFO);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::WARNING);
        AppLogger::Logger::Log(std::to_string(i), AppLogger::ERROR);
    }
    ulong end_ns = CurrentTime();
    ulong duration_ms = (end_ns - start_ns) / 1.0e6;
    double logger_freq = (n_messages * 4.0) / (duration_ms / 1.0e3);
    std::cout << "enqueueing 400,000 log messages took " << duration_ms << " ms, on average " << duration_ms / (n_messages * 4.0) << " ms per message" << std::endl;
    std::cout << "logger frequency: " << logger_freq << "Hz " << std::endl;
    std::cout << "flushing logger..." << std::endl;
    AppLogger::Logger::Flush(); // make sure stdout and file get flushed before terminating

    ulong end_close_ns = CurrentTime();
    ulong full_duration_ms = (end_close_ns - end_ns) / 1.0e6;

    std::cout << "Wrote " << n_messages << " x4 messages to the log file in " << full_duration_ms << " ms" << std::endl;

    ASSERT_GE(logger_freq, 200000);
    ASSERT_LE(full_duration_ms, 2500);
    ASSERT_TRUE(std::filesystem::exists(logfile));
}