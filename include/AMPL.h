#pragma once

// Disable debug stdout for Apriltag library
#define NODEBUG 1

#include <string>
#include <vector>
#include <csignal>

#include "LocalizationWorker.h"
#include "WebServerWorker.h"
#include "TDCamWorker.h"
#include "NTWorker.h"
#include "ConfigParser.h"
#include "Logger.h"

class AMPL{
public:
    AMPL (AMPL const&) = delete;

    static AMPL& GetInstance();

    void Setup(const std::string& config_file, const std::string& fmap_file);

    void Start();

    RobotPose GetRobotPose();

    void Join();

    bool Stop();

    static void StaticSignalCallback(int signum);

    void SignalCallback(int signum);

private:
    AMPL() = default;

    LocalizationWorker* _l_w;
    std::vector<Worker*> _workers_t;
};