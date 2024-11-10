
// No debug messages from the apriltag library

#define NODEBUG 1
//#define DEBUG 0


#include <vector>
#include <csignal>

#include "LocalizationWorker.h"
#include "WebServerWorker.h"
#include "TDCamWorker.h"
#include "NTWorker.h"
#include "ConfigParser.h"
#include "Logger.h"


std::vector<Worker*> workers_t;

void signal_callback(int signum){
    AppLogger::Logger::Log("Caught CTRL-C, exiting...");
    for (Worker* t: workers_t){
        t->Stop();
        delete t;
    }
    exit(signum);
}


int main(int argc, char *argv[])
{
    AppLogger::Logger::SetVerbosity(AppLogger::INFO);
    AppLogger::Logger::Log("Starting Multicam Apriltag Localization", AppLogger::INFO);

    // Register signal handler
    signal(SIGINT, signal_callback);

    // Parse map and configuration files
    AMPLParams params = ConfigParser::ParseConfig("../config.yml");
    std::vector<CamParams>& c_params = params.cam_params;
    TagLayoutParser::ParseConfig("../at14_6.fmap");

    // Create localization worker
    workers_t.emplace_back(new LocalizationWorker());
    LocalizationWorker* l_w = (LocalizationWorker*) workers_t.back();
    l_w->LogStats(true);

    // Create webserver worker
    workers_t.emplace_back(new WebServerWorker(8080));
    WebServerWorker* w_w = (WebServerWorker*) workers_t.back();

    // Create NetworkTables worker
    workers_t.emplace_back(new NTWorker(params.team_num));
    NTWorker* w_nt = (NTWorker*) workers_t.back();
    w_nt->RegisterPoseCallback([l_w]() -> RobotPose {return l_w->GetRobotPose();});

    // Create camera workers
    for (CamParams& p: c_params){
        workers_t.emplace_back(new TDCamWorker(p, [l_w](TagArray& raw_tags) -> bool {return l_w->QueueTags(raw_tags);}, false));
        TDCamWorker* this_cam_worker = (TDCamWorker*) workers_t.back();
        w_w->RegisterMatFunc([this_cam_worker]() -> cv::Mat {return this_cam_worker->GetAnnotatedIm();});
    }

    // Start all camera workers
    for (Worker* w: workers_t){
        w->Start();
    }
    AppLogger::Logger::Log("All workers have been started");

    // Wait until the tag detection threads are finished
    for (Worker* w: workers_t){
        w->Join();
        delete w;
    }

    AppLogger::Logger::Log("All workers finished");

    return 0;
}
