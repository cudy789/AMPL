#include "AMPL.h"

void AMPL::Setup(const std::string& config_file, const std::string& fmap_file) {
    AppLogger::Logger::SetVerbosity(AppLogger::INFO);
    AppLogger::Logger::Log("Starting Apriltag Multicam Pose Localization", AppLogger::INFO);

    // Register signal handler
    signal(SIGINT, AMPL::AMPL::StaticSignalCallback);

    // Parse map and configuration files
    AMPLParams params = ConfigParser::ParseConfig(config_file);
    std::vector<CamParams>& c_params = params.cam_params;
    TagLayoutParser::ParseConfig(fmap_file);

//    // Create localization worker
    _workers_t.emplace_back(new LocalizationWorker());
    LocalizationWorker* l_w = (LocalizationWorker*) _workers_t.back();
    l_w->LogStats(true);

    // Create webserver worker
    _workers_t.emplace_back(new WebServerWorker(8080));
    WebServerWorker* w_w = (WebServerWorker*) _workers_t.back();

//    // Create NetworkTables worker
//    _workers_t.emplace_back(new NTWorker(params.team_num));
//    NTWorker* w_nt = (NTWorker*) _workers_t.back();
//    w_nt->RegisterPoseCallback([l_w]() -> RobotPose {return l_w->GetRobotPose();});

    // Create camera workers
    for (CamParams& p: c_params){
        _workers_t.emplace_back(new TDCamWorker(p, [l_w](TagArray& raw_tags) -> bool {return l_w->QueueTags(raw_tags);}, false));
        TDCamWorker* this_cam_worker = (TDCamWorker*) _workers_t.back();
        w_w->RegisterMatFunc([this_cam_worker]() -> cv::Mat {return this_cam_worker->GetAnnotatedIm();});
    }
}

AMPL &AMPL::GetInstance() {
    static AMPL instance; // instantiated on first call, guaranteed to be destroyed
    return instance;
}

void AMPL::Start(){
    // Start all workers
    for (Worker* w: _workers_t){
        w->Start();
    }
    AppLogger::Logger::Log("All workers have been started");
}

void AMPL::Join(){
    // Wait until the tag detection threads are finished
    for (Worker* w: _workers_t){
        w->Join();
        delete w;
    }
    AppLogger::Logger::Log("All workers finished");
}

bool AMPL::Stop(){
    bool all_stopped = true;
    for (Worker* w: _workers_t){
        all_stopped = w->Stop();
    }
    return all_stopped;
}

void AMPL::StaticSignalCallback(int signum) {
    GetInstance().SignalCallback(signum);
}

void AMPL::SignalCallback(int signum) {
    AppLogger::Logger::Log("Caught CTRL-C, exiting...");
    for (Worker* t: _workers_t){
        AppLogger::Logger::Log("in AMPL, trying to stop " + t->GetName());
        t->Stop();
        delete t;
    }
    exit(signum);
}