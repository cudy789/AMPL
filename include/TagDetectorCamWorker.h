#pragma once

#include <thread>
#include <bits/stdc++.h>

#include "TagDetectorCamera.h"

class TagDetectorCamWorker: public TagDetectorCamera {
public:

    TagDetectorCamWorker(getopt_t *getopt, int cameraId, const Eigen::Matrix3d &rCameraRobot,
                         const Eigen::Vector3d &tCameraRobot, std::function<bool(TagArray&)> queue_tags_callback);

    void Start();

    void join();

    void Run();


protected:

    std::thread _t_worker;
    bool _show_im;
    std::function<bool(TagArray&)> _queue_tags_callback;


};