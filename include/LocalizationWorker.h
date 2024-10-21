//
// Created by corey on 8/27/24.
//

#pragma once

#include <semaphore>
#include <vector>

#include "Pose.h"
#include "Localization.h"
#include "Worker.h"

inline int WORK_PERIOD_MS = 10; // Run at 1/x Hz, in this case, 100Hz
inline int INTER_CAMERA_DELTA_MS = 50; // If the time delta between the same tag being seen by 2+ different cameras is less than
// this value, then lets use these multiple tag poses from the different cameras to compute the single true tag pose



class LocalizationWorker: public LocalizationFilter, public Worker{

public:
    LocalizationWorker();

    bool QueueTags(TagArray& raw_tagarray);

    bool QueueTag(Pose raw_pose);

    RobotPose GetRobotPose();

    Pose GetTagPose(int tag_id);

    void LogStats(bool log_stats);


    private:

    void Init() override;
    void Execute() override;


    std::binary_semaphore _raw_tag_sem{1};
    TagArray _raw_tag_poses;
    TagArray _fresh_tag_poses;
    std::vector<int> _fresh_unique_tags = std::vector<int> (static_cast<int>(NUM_TAG_IDS), 0);
    std::vector<int> _last_unique_tags = std::vector<int> (static_cast<int>(NUM_TAG_IDS), 0);

    std::binary_semaphore _robot_pose_sem{1};
    Pose _robot_pose;

    bool _log_stats = false;
    ulong _last_log_time_ns = CurrentTime();
    ulong _log_period_ns = 5e8; // 500 ms logging rate


};
