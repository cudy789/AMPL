#include "AMPL.h"
#include "CalibrationCamWorker.h"

void AMPL::Setup(const std::string& config_file) {
    AppLogger::Logger::SetVerbosity(AppLogger::INFO);
    AppLogger::Logger::Log("Starting Apriltag Multicam Pose Localization", AppLogger::INFO);

    // Register signal handler
    signal(SIGINT, AMPL::AMPL::StaticSignalCallback);

    // Parse map and configuration files
    AMPLParams params = ConfigParser::ParseConfig(config_file);
    std::vector<CamParams>& c_params = params.cam_params;
    std::map<int, Pose_single> tag_layout = TagLayoutParser::ParseConfig(params.fmap_file);

    // Create localization worker
    _l_w = new LocalizationWorker(params.pose_logging);
    _workers_t.emplace_back(_l_w);
    _l_w->LogStats(true);

    // Create webserver worker
    WebServerWorker* w_w = new WebServerWorker(8080);
    w_w->RegisterRobotPoseFunc([this]() -> RobotPose {return _l_w->GetRobotPose();});
    _workers_t.emplace_back(w_w);

    // Create NetworkTables worker
    if (params.team_num > 0){
        NTWorker* w_nt = new NTWorker(params.team_num);
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
                                                           params.video_recording);
            w_w->RegisterMatFunc([this_cam_worker]() -> cv::Mat {return this_cam_worker->GetAnnotatedIm();});
            _workers_t.emplace_back(this_cam_worker);
        }
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

void AMPL::Calibrate() {

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