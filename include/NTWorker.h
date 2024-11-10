#pragma once

#include "Worker.h"
#include "Pose.h"

#include <networktables/DoubleArrayTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

class NTWorker: public Worker{

public:

    NTWorker();
    NTWorker(int team_num);
    NTWorker(std::string hostname);

    void RegisterPoseCallback(const std::function<RobotPose()>& pose_callback);
    bool IsConnected();

private:

    void Init() override;
    void Execute() override;

    int _team_num = -1;
    std::string _hostname;

    nt::NetworkTableInstance _nt_instance;
    std::shared_ptr<nt::NetworkTable> _nt_table;

    nt::DoubleArrayPublisher _position;
    nt::DoubleArrayPublisher _orientation;

    std::function<RobotPose()> _pose_callback;
};