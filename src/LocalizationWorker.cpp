#include <iostream>
#include <bits/stdc++.h>

#include "LocalizationWorker.h"
#include "MatrixHelpers.h"
#include "TrajectoryLogger.h"

LocalizationWorker::LocalizationWorker(bool pose_logging) : Worker{"Localization worker", AppLogger::SEVERITY::DEBUG},
        Localization{new MeanLocalizationStrategy()},
        _pose_logging(pose_logging)
{}

bool LocalizationWorker::QueueTag(Pose raw_pose) {
    if(_raw_tag_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(50))){
        _raw_tag_poses.AddTag(raw_pose);
        _raw_tag_sem.release();
        return true;
    }
    return false;
}

bool LocalizationWorker::QueueTags(TagArray& raw_tagarray){
    if(_raw_tag_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(50))){
        for (std::vector<Pose>& v: raw_tagarray.data){
            for (Pose& p: v){
                _raw_tag_poses.AddTag(p);
            }
        }
        _raw_tag_sem.release();
        return true;
    }
    return false;
}

RobotPose LocalizationWorker::GetRobotPose() {
    RobotPose ret_pose;
    if(_robot_pose_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))) {
        ret_pose = _filtered_pose;
        _robot_pose_sem.release();
    }

    return ret_pose;
}

void LocalizationWorker::LogStats(bool log_stats){
    _log_stats = log_stats;
}

void LocalizationWorker::Execute() {
    // ==================== Get raw tags ====================

    // Get lock for new tags
    if (_raw_tag_sem.try_acquire()) {
        // Get a local copy of the raw tags
        TagArray local_raw_tag_poses = _raw_tag_poses;
        _raw_tag_poses.ClearAll();
        _raw_tag_sem.release();


        // Add fresh poses
        for (int i = 0; i < local_raw_tag_poses.data.size(); i++) {
            std::vector<Pose> &r_v = local_raw_tag_poses.data[i];
            for (Pose& r_p: r_v) {
                _fresh_tag_poses.AddTag(r_p);
            }
        }

        // Clear TagArray of stale tag poses
        int stale_tags = _fresh_tag_poses.ClearStale();

        // ==================== Log unique new/lost tags ====================

        _fresh_unique_tags = std::vector<int>(NUM_TAG_IDS, 0);

        // Find all of the unique tag ids from the fresh tag poses
        for(int i=0; i<NUM_TAG_IDS; i++){
            if (!_fresh_tag_poses.data[i].empty()){
                _fresh_unique_tags[i]++;
            }
        }
        for (int i = 0; i < NUM_TAG_IDS; i++) {
            int f = _fresh_unique_tags[i];
            int l = _last_unique_tags[i];
            if (f != l) {
                if (f > 0) {
                    AppLogger::Logger::Log("Started tracking tag " + std::to_string(i+1));
                } else {
                    AppLogger::Logger::Log("Lost tracking on tag " + std::to_string(i+1));
                }
            }
        }
        _last_unique_tags = _fresh_unique_tags;

        // ==================== Compute new robot state ====================

        // Get lock to grab latest robot pose
        if(_robot_pose_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))) {
            RobotPose local_filtered_pose = _filtered_pose;
            _robot_pose_sem.release();
            // Compute updated pose
            _strategy->Compute(_fresh_tag_poses, local_filtered_pose);

            // Log pose
            if (_pose_logging){
                AppLogger::TrajectoryLogger::Log(local_filtered_pose);
            }

            // Get lock to reassign updated pose
            if (_robot_pose_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))) {
                _filtered_pose = local_filtered_pose;
                _robot_pose_sem.release();
            }
        }

    }

    if ((CurrentTime() - _last_log_time_ns) > _log_period_ns){
        if (_log_stats){
//            AppLogger::Logger::Log("Robot pose: " + to_string(GetRobotPose()), AppLogger::DEBUG);
            AppLogger::Logger::Log("")
        }
        _last_log_time_ns = CurrentTime();
    }


}
