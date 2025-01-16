#include "MAPLE.h"
#include "CalibrationCamWorker.h"

void MAPLE::Setup(const std::string& config_file) {
    AppLogger::Logger::SetVerbosity(AppLogger::INFO);
    AppLogger::Logger::Log("Starting Multicamera Apriltag Pose Localization and Estimation (MAPLE)", AppLogger::INFO);

    // Register signal handler
    signal(SIGINT, MAPLE::MAPLE::StaticSignalCallback);

    // Parse map and configuration files
    _params = ConfigParser::ParseConfig(config_file);
    std::vector<CamParams>& c_params = _params.cam_params;
    std::map<int, Pose_single> tag_layout = TagLayoutParser::ParseConfig(_params.fmap_file);

    // Create localization worker
    _l_w = new LocalizationWorker(_params.pose_logging);
    _workers_t.emplace_back(_l_w);
    _l_w->LogStats(true);

    // Create webserver worker
    WebServerWorker* w_w = new WebServerWorker(8080);
    w_w->RegisterRobotPoseFunc([this]() -> RobotPose {return _l_w->GetRobotPose();});
    _workers_t.emplace_back(w_w);

    // Create NetworkTables worker
    if (_params.team_num > 0){
        NTWorker* w_nt = new NTWorker(_params.team_num);
        _workers_t.emplace_back(w_nt);
        w_nt->RegisterPoseCallback([this]() -> RobotPose {return _l_w->GetRobotPose();});
    } else{
        AppLogger::Logger::Log("Not starting NetworkTables, invalid team number provided", AppLogger::SEVERITY::WARNING);
    }

    // Create camera workers
    for (CamParams& p: c_params){
        if (p.calibrate){
            CalibrationCamWorker* this_cam_worker = new CalibrationCamWorker(p);
            w_w->RegisterMatFunc([this_cam_worker]() -> cv::Mat {return this_cam_worker->GetAnnotatedIm();});
            _workers_t.emplace_back(this_cam_worker);
        } else{
            TDCamWorker* this_cam_worker = new TDCamWorker(p, tag_layout, [this](TagArray& raw_tags) -> bool {return _l_w->QueueTags(raw_tags);},
                                                           _params.video_recording);
            w_w->RegisterMatFunc([this_cam_worker]() -> cv::Mat {return this_cam_worker->GetAnnotatedIm();});
            _workers_t.emplace_back(this_cam_worker);
        }
    }
}

MAPLE &MAPLE::GetInstance() {
    static MAPLE instance; // instantiated on first call, guaranteed to be destroyed
    return instance;
}

void MAPLE::Start(){
    // Start all workers
    for (Worker* w: _workers_t){
        w->Start();
    }
    AppLogger::Logger::Log("All workers have been started");
}

void MAPLE::Calibrate() {

}

RobotPose MAPLE::GetRobotPose() {
    return _l_w->GetRobotPose();
}

void MAPLE::Join(){
    // Wait until the tag detection threads are finished
    for (Worker* w: _workers_t){
        w->Join();
        delete w;
    }
    AppLogger::Logger::Log("All workers finished");
}

bool MAPLE::Stop(){
    bool all_stopped = true;
    for (Worker* w: _workers_t){
        all_stopped = w->Stop();
    }
    return all_stopped;
}

void MAPLE::StaticSignalCallback(int signum) {
    GetInstance().SignalCallback(signum);
}

void MAPLE::SignalCallback(int signum) {
    AppLogger::Logger::Log("Caught CTRL-C, exiting...");
    for (Worker* t: _workers_t){
        t->Stop();
        delete t;
    }
    exit(signum);
}