//
// Created by corey on 8/27/24.
//

#pragma once

#include <semaphore>
#include <vector>

#include "Pose.h"
#include "Localization.h"

inline int WORK_PERIOD_MS = 10; // Run at 1/x Hz, in this case, 100Hz
inline int INTER_CAMERA_DELTA_MS = 50; // If the time delta between the same tag being seen by 2+ different cameras is less than
// this value, then lets use these multiple tag poses from the different cameras to compute the single true tag pose

using namespace Localization;


class LocalizationWorker{

public:
    LocalizationWorker() = default;

    void Start();

    void join();

    bool QueueTags(TagArray& raw_tagarray);

    bool QueueTag(TagPose raw_pose);

    Pose GetRobotPose();

    TagPose GetTagPose(int tag_id);



private:
    void Run();
    std::thread _t_worker;

    std::binary_semaphore _raw_tag_sem{1};
    TagArray _raw_tag_poses;
    TagArray _fresh_tag_poses;
    std::vector<TagPose> _computed_tag_poses{NUM_TAG_IDS};
    Pose _robot_pose;


};
