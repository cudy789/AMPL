#pragma once

#include <filesystem>

#include "Logger.h"


inline void SetupLogger(const std::string& testname){
    std::string filepath = "./test_" + testname + ".txt";
    std::filesystem::remove(filepath);

    AppLogger::Logger::SetFilepath(filepath);
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);

    AppLogger::Logger::SetStdout(true);
    AppLogger::Logger::SetFileout(true);

}