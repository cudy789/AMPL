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

    // Create localization worker
    _l_w = new LocalizationWorker;
    _workers_t.emplace_back(_l_w);
    _l_w->LogStats(true);

    // Create webserver worker
    WebServerWorker* w_w = new WebServerWorker(8080);
    _workers_t.emplace_back(w_w);

    // Create NetworkTables worker
    if (params.team_num > 0){
        NTWorker* w_nt = new NTWorker();
        _workers_t.emplace_back(w_nt);
        w_nt->RegisterPoseCallback([this]() -> RobotPose {return _l_w->GetRobotPose();});
    } else{
        AppLogger::Logger::Log("Not starting NetworkTables, invalid team number provided", AppLogger::SEVERITY::WARNING);
    }

    // Create camera workers
    for (CamParams& p: c_params){
        TDCamWorker* this_cam_worker = new TDCamWorker(p, [this](TagArray& raw_tags) -> bool {return _l_w->QueueTags(raw_tags);}, false);
        _workers_t.emplace_back(this_cam_worker);
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

RobotPose AMPL::GetRobotPose() {
    return _l_w->GetRobotPose();
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
        t->Stop();
        delete t;
    }
    exit(signum);
}