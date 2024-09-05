#include <iostream>
#include <bits/stdc++.h>

#include "LocalizationWorker.h"


LocalizationWorker::LocalizationWorker() : Worker("Localization worker"){

}

bool LocalizationWorker::QueueTag(TagPose raw_pose) {
    if(_raw_tag_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(50))){
        _raw_tag_poses.data[raw_pose.tag_id-1].push_back(raw_pose);
        _raw_tag_sem.release();
        return true;
    }
    return false;
}

bool LocalizationWorker::QueueTags(TagArray& raw_tagarray){
    if(_raw_tag_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(50))){
        for (std::vector<TagPose>& v: raw_tagarray.data){
            for (TagPose p: v){
                _raw_tag_poses.data[p.tag_id-1].push_back(p);
            }
        }
        _raw_tag_sem.release();
        return true;
    }
    return false;
}

void LocalizationWorker::Init() {

}

void LocalizationWorker::Execute() {
    std::vector<TagPose> last_tag_poses{NUM_TAG_IDS};

    if (_raw_tag_sem.try_acquire()) {
        // Get a local copy of the raw tags
        TagArray local_raw_tag_poses = _raw_tag_poses;
        _raw_tag_poses.ClearAll();
        _raw_tag_sem.release();


        // Add fresh poses
        for (int i = 0; i < local_raw_tag_poses.data.size(); i++) {
            std::vector<TagPose> &r_v = local_raw_tag_poses.data[i];
            for (TagPose r_p: r_v) {
                _fresh_tag_poses.data[i].push_back(r_p);
            }
        }

        // Clear TagArray of stale tag poses
        int stale_tags = _fresh_tag_poses.ClearStale();

        // Disambiguate tags with multiple poses
        _computed_tag_poses = DisambiguateTags(_fresh_tag_poses);

        for (int i = 0; i < NUM_TAG_IDS; i++) {
            TagPose &c_t = _computed_tag_poses[i];
            TagPose &l_t = last_tag_poses[i];
            if (c_t.tag_id != l_t.tag_id) {
                if (c_t.tag_id > 0) {
                    AppLogger::Logger::Log("Started tracking tag " + std::to_string(c_t.tag_id));
                } else {
                    AppLogger::Logger::Log("Lost tracking on tag " + std::to_string(c_t.tag_id));
                }
            }
        }


        last_tag_poses = _computed_tag_poses;


        // From each of the tags, calculate the robot's position in the world
        // TODO ^^
    }

}
