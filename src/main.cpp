#include <iomanip>
#include <thread>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <csignal>

#include "MatrixHelpers.h"
#include "LocalizationWorker.h"
#include "TDCamWorker.h"

#include "opencv2/opencv.hpp"
#include "Logger.h"




std::vector<Worker> workers_t;

void signal_callback(int signum){
    AppLogger::Logger::Log("Caught CTRL-C, exiting...");
    for (Worker& t: workers_t){
        t.Stop();
    }
    exit(signum);
}


int main(int argc, char *argv[])
{
    AppLogger::Logger::SetVerbosity(AppLogger::DEBUG);
    AppLogger::Logger::Log("Starting Multicam Apriltag Localization", AppLogger::INFO);

    // Register signal handler
    signal(SIGINT, signal_callback);



//    ThreadWork(getopt, 0);
//    thread t_cam1(&ThreadWork, getopt, 0);
//    std::vector<int> cam_ids = GetAttachedCamIDs(15);
//    Eigen::Matrix3d R_camera_robot = CreateRotationMatrix({90, 0, 0});
//    Eigen::Vector3d t_camera_robot(0, 0, 0.55);

    std::vector<int> cam_ids = {2};
//    std::vector<thread> cam_threads;
//    std::vector<TagDetectorCamWorker> cam_workers;
//    LocalizationWorker l_worker;

    Eigen::Matrix3d rotmat;
    rotmat << 1, 0, 0,
            0, 0, 1,
            0, -1, 0;
    // roll, -90 is straight ahead

//    std::vector<Eigen::Matrix3d> R_camera_robots = {CreateRotationMatrix({-90, 0, 0})};
    std::vector<Eigen::Matrix3d> R_camera_robots = {
            CreateRotationMatrix({-90, 0, 25}), // 25 degrees w.r.t robot Z for right cam
            CreateRotationMatrix({-90, 0, 0}), // center cam
            CreateRotationMatrix({-90, 0, 0}) // center cam
    };



//    std::vector<Eigen::Matrix3d> R_camera_robots = {rotmat, rotmat};
//    std::vector<Eigen::Vector3d> t_camera_robots = {{0, 0, 0}}; // center cam
    std::vector<Eigen::Vector3d> t_camera_robots = {
            {0.14, -0.03, 0}, // right cam
            {0, 0, 0}, // center cam
            {0, 0, 0} // center cam
    };


    workers_t.emplace_back(LocalizationWorker());

    // Start all workers
    for (Worker& w: workers_t){
        w.Start();
    }
    AppLogger::Logger::Log("All workers have been started");



    // Wait until the tag detection threads are finished
    for (Worker& w: workers_t){
        w.Join();
    }
    AppLogger::Logger::Log("All workers finished");

    // Wait until localization is finished (never)
//    l_worker.join();

    return 0;
}
