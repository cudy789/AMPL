#include <vector>
#include <csignal>

#include "LocalizationWorker.h"
#include "WebServerWorker.h"
#include "TDCamWorker.h"
#include "ParamParser.h"
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

    // Create localization worker
    workers_t.emplace_back(new LocalizationWorker());
    LocalizationWorker* l_w = (LocalizationWorker*) workers_t[0];
    l_w->LogStats(true);

    // Create webserver worker
    workers_t.emplace_back(new WebServerWorker(8080));
    WebServerWorker* w_w = (WebServerWorker*) workers_t[1];

    // Parse map and camera configuration for camera workers
    std::vector<CamParams> c_params = ParamParser::ParseConfig("../config.yml");
    TagLayoutParser::ParseConfig("../at14.fmap");

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

    //TODO randomly crashing -> corrupt size vs. prev_size was an error message

    return 0;
}
