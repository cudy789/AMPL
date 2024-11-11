#include "NTWorker.h"

NTWorker::NTWorker() :
        Worker{"NetworkTables worker"},
        _nt_instance{nt::NetworkTableInstance::GetDefault()} {

    _nt_table = _nt_instance.GetTable("AMPL");

    _position = _nt_table->GetDoubleArrayTopic("position").Publish();
    _orientation = _nt_table->GetDoubleArrayTopic("orientation").Publish();
}

NTWorker::NTWorker(int team_num) :  NTWorker() { _team_num=team_num; }

NTWorker::NTWorker(std::string hostname) :  NTWorker() { _hostname=hostname; }

void NTWorker::RegisterPoseCallback(const std::function<RobotPose()>& pose_callback) {
    _pose_callback = pose_callback;
}

bool NTWorker::IsConnected(){
    return _nt_instance.IsConnected();
}

void NTWorker::Init() {
    _nt_instance.StartClient4("AMPL client");

    if (_team_num != -1){
        AppLogger::Logger::Log("NTWorker connecting using team number " + to_string(_team_num));
        _nt_instance.SetServerTeam(_team_num);
    } else {
        AppLogger::Logger::Log("NTWorker connecting to " + _hostname + ":" + to_string(NT_DEFAULT_PORT4));
        _nt_instance.SetServer(_hostname, NT_DEFAULT_PORT4);
    }

    sleep(2);

    if (_nt_instance.IsConnected()){
        AppLogger::Logger::Log("NTWorker successfully connected");
    } else{
        AppLogger::Logger::Log("NTWorker could not connect to NetworkTables server, retrying...", AppLogger::SEVERITY::WARNING);
        Stop(false);
        sleep(10);
    }
}

void NTWorker::Execute() {
    RobotPose new_pose = _pose_callback();

    std::vector<double> new_position = {new_pose.global.T[0], new_pose.global.T[1], new_pose.global.T[2]};

    Eigen::Vector3d eig_new_orientation = RotationMatrixToRPY(new_pose.global.R);
    std::vector<double> new_orientation = {eig_new_orientation[0], eig_new_orientation[1], eig_new_orientation[2]};


    _position.Set(new_position);
    _orientation.Set(new_orientation);

}