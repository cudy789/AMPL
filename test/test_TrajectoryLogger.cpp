#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>

#include "TrajectoryLogger.h"
#include "TimeUtils.h"
#include "Pose.h"
#include "common.h"

void SetupTrajectoryLogger(const std::string& test_name){
    std::filesystem::remove("test_" + test_name + ".csv");
    AppLogger::TrajectoryLogger::SetFilepath("test_" + test_name + ".csv");
    AppLogger::TrajectoryLogger::SetStdout(true);
    AppLogger::TrajectoryLogger::SetFileout(true);
}

std::vector<RobotPose> LogTrajectory(int num_msgs){

    double increment = 90.0 / num_msgs;

    std::vector<RobotPose> trajectories;
    for(double i=0.0; i<=90.0; i=i+increment){
        trajectories.emplace_back(Pose_single{.R=CreateRotationMatrix(Eigen::Vector3d(0,i,0))});
    }

    return trajectories;

}
TEST(TrajectoryLogger, SeparateInstances){
    std::string test_name = "trajectory_logger_SeparateInstances";
    SetupTrajectoryLogger(test_name);
    SetupLogger(test_name);

    AppLogger::Logger::Flush();
    AppLogger::TrajectoryLogger::Flush();

    ASSERT_NE(AppLogger::Logger::GetFilePath(), AppLogger::TrajectoryLogger::GetFilePath());

}

TEST(TrajectoryLogger, HelloWorldStdOut){
    std::string test_name = "trajectory_logger_HelloWorldStdOut";
    SetupTrajectoryLogger(test_name);

    AppLogger::TrajectoryLogger::SetFileout(false);
    for(const RobotPose& r: LogTrajectory(100)){
        AppLogger::TrajectoryLogger::Log(r);
    }

    AppLogger::TrajectoryLogger::Flush(); // make sure stdout and file get flushed before terminating
    ASSERT_FALSE(std::filesystem::exists(test_name));
}

TEST(TrajectoryLogger, HelloWorldFileOut){
    std::string test_name = "trajectory_logger_HelloWorldFileOut";
    SetupLogger(test_name);
    SetupTrajectoryLogger(test_name);

    AppLogger::TrajectoryLogger::SetStdout(false);
    for(const RobotPose& r: LogTrajectory(100)){
        AppLogger::TrajectoryLogger::Log(r);
    }
    AppLogger::Logger::Log("is trajectorylogger stdout enabled? " + to_string(AppLogger::TrajectoryLogger::GetStdout()));
    AppLogger::Logger::Log("is trajectorylogger fileout enabled? " + to_string(AppLogger::TrajectoryLogger::GetFileout()));
    AppLogger::Logger::Log("is logger fileout enabled? " + to_string(AppLogger::Logger::GetFileout()));

    AppLogger::TrajectoryLogger::Flush(); // make sure stdout and file get flushed before terminating
    ASSERT_TRUE(std::filesystem::exists("test_" + test_name + ".csv"));
    AppLogger::Logger::Flush();

}

TEST(TrajectoryLogger, HighFreqFileout){
    std::string test_name = "trajectory_logger_HighFreqFileout";
    SetupLogger(test_name);
    SetupTrajectoryLogger(test_name);

    int n_messages = 100000;

    AppLogger::TrajectoryLogger::SetStdout(false);

    ulong start_ns = CurrentTime();
    for(const RobotPose& r: LogTrajectory(n_messages)){
        AppLogger::TrajectoryLogger::Log(r);
    }
    ulong end_ns = CurrentTime();
    ulong duration_ms = (end_ns - start_ns) / 1.0e6;
    double logger_freq = (n_messages) / (duration_ms / 1.0e3);
    AppLogger::Logger::Log("Enqueueing " + to_string(n_messages) + " trajectory messages took "
        + to_string(duration_ms) + " ms, on average "
        + to_string((double)duration_ms / n_messages) + " ms per message");
    AppLogger::Logger::Log("Trajectory logger frequency: " + to_string(logger_freq) + "hz");

    AppLogger::TrajectoryLogger::Flush(); // make sure stdout and file get flushed before terminating

    ASSERT_GE(logger_freq, 10000);
    ASSERT_TRUE(std::filesystem::exists("test_" + test_name + ".csv"));
    AppLogger::Logger::Flush();
}