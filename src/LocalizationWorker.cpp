#include <iostream>
#include <bits/stdc++.h>

#include "LocalizationWorker.h"

void LocalizationWorker::Start() {
    std::cout << "Starting localization thread" << std::endl;
    _t_worker = std::thread([this]() {this->Run();});
}

void LocalizationWorker::join() {
    _t_worker.join();
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

void LocalizationWorker::Run() {
    std::vector<ulong> runtimes;
    ulong missed_lock = 0;
    ulong last_processed_t = CurrentTime();

    std::vector<TagPose> last_tag_poses{NUM_TAG_IDS};


    while (true) {

        if ((CurrentTime() - last_processed_t) > WORK_PERIOD_MS * 1.0e6) {
            if (_raw_tag_sem.try_acquire()){
                missed_lock = 0;
                // Get a local copy of the raw tags
                TagArray local_raw_tag_poses = _raw_tag_poses;
                _raw_tag_poses.ClearAll();
                _raw_tag_sem.release();




                // Add fresh poses
                for (int i = 0; i < local_raw_tag_poses.data.size(); i++) {
                    std::vector<TagPose>& r_v = local_raw_tag_poses.data[i];
                    for (TagPose r_p: r_v) {
                        _fresh_tag_poses.data[i].push_back(r_p);
                    }
                }

                // Clear TagArray of stale tag poses
                int stale_tags = _fresh_tag_poses.ClearStale();

                // Disambiguate tags with multiple poses
                _computed_tag_poses = DisambiguateTags(_fresh_tag_poses);

                for (int i=0; i < NUM_TAG_IDS; i++){
                    TagPose& c_t = _computed_tag_poses[i];
                    TagPose& l_t = last_tag_poses[i];
                    if (c_t.tag_id != l_t.tag_id){
                        if (c_t.tag_id > 0){
                            std::cout << "Tracking tag " << c_t.tag_id << std::endl;
                        } else{
                            std::cout << "Lost tracking on tag " << l_t.tag_id << std::endl;
                        }
                    }
                }


                last_tag_poses = _computed_tag_poses;


                // From each of the tags, calculate the robot's position in the world
                // TODO ^^

                runtimes.push_back(CurrentTime() - last_processed_t);
                last_processed_t = CurrentTime();
                if (runtimes.size() == 100){
//                    std::cout << "Average localization worker execution time: " << (std::accumulate(runtimes.begin(), runtimes.end(), 0.0) / (1.0e6 * runtimes.size())) << "ms" << std::endl;
//                    std::cout << "\t Max: " << *std::max_element(runtimes.begin(), runtimes.end()) / 1.0e6 << "ms" << std::endl;
                    runtimes.clear();
                }

            } else{
                missed_lock++;
                std::cout << "Didn't acquire lock, missed lock " << missed_lock << " times in a row" << std::endl;
            }
        }
        std::this_thread::yield();

    }

}
