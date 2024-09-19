#include <cmath>

#include "Localization.h"
#include "Logger.h"

bool MeanLocalizationStrategy::Compute(TagArray &fresh_poses, RobotPose &filtered_pose) {
    // Compute the mean of the global poses for each tag
    Pose_base avg_filtered_pose;
    int num_poses = 0;

    // Compute average
    for (std::vector<PoseCv>& v: fresh_poses.data){
        for (PoseCv& p: v){
            avg_filtered_pose += p.global;
            num_poses++;
        }
    }
    if (num_poses > 0) {
        avg_filtered_pose /= num_poses;
    } else{
        return true;
    }

//    // Compute MSE
//    Pose_base mse;
//    for (std::vector<PoseCv>& v: fresh_poses.data){
//        for (PoseCv& p: v){
//            avg_filtered_pose - p.global;
//        }
//    }


    filtered_pose.global = avg_filtered_pose;

    return true;
}