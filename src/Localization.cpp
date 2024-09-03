#include <cmath>

#include "Localization.h"


std::vector<std::vector<double>> Stats(std::vector<TagPose>& tag_poses){
    int count = tag_poses.size();

    std::vector<double> mean_xyz = {0,0,0};
    std::vector<double> mean_rpy = {0,0,0};
    std::vector<double> std_dev_xyz = {0,0,0};
    std::vector<double> std_dev_rpy = {0,0,0};

    if (count > 0) {

        // Calculate mean
        for (TagPose& p: tag_poses) {
            mean_xyz[0] += p.x;
            mean_xyz[1] += p.y;
            mean_xyz[2] += p.z;

            mean_rpy[0] += p.roll;
            mean_rpy[1] += p.pitch;
            mean_rpy[2] += p.yaw;
        }
        for (double &v: mean_xyz) {
            v /= count;
        }
        for (double &v: mean_rpy) {
            v /= count;
        }

        // Calculate variance = SUM(x - x_mean)^2

        for (TagPose p: tag_poses) {
            std_dev_xyz[0] += (p.x - mean_xyz[0]) * (p.x - mean_xyz[0]);
            std_dev_xyz[1] += (p.y - mean_xyz[1]) * (p.y - mean_xyz[1]);
            std_dev_xyz[2] += (p.z - mean_xyz[2]) * (p.z - mean_xyz[2]);

            std_dev_rpy[0] += (p.roll - mean_rpy[0]) * (p.roll - mean_rpy[0]);
            std_dev_rpy[1] += (p.pitch - mean_rpy[1]) * (p.pitch - mean_rpy[1]);
            std_dev_rpy[2] += (p.yaw - mean_rpy[2]) * (p.yaw - mean_rpy[2]);
        }
        // Calculate standard devation = sqrt(variance)
        for (double &v: std_dev_rpy) {
            v = sqrt(v/count);
        }
        for (double &v: std_dev_xyz) {
            v = sqrt(v/count);
        }
    }

    return {mean_xyz, std_dev_xyz, mean_rpy, std_dev_rpy};

}

std::vector<TagPose> DisambiguateTags(TagArray& fresh_tag_poses){
    std::vector<TagPose> computed_tag_poses{NUM_TAG_IDS};

    // Just average out the values of multiple poses. TODO do something smarter!
    for (int i = 0; i < NUM_TAG_IDS; i++) {
        std::vector<TagPose> &r_v = fresh_tag_poses.data[i];

        if (r_v.empty()) continue;

        std::vector<std::vector<double>> tag_stats = Stats(r_v);

        computed_tag_poses[i].tag_id = r_v[0].tag_id;

        computed_tag_poses[i].x = tag_stats[0][0];
        computed_tag_poses[i].y = tag_stats[0][1];
        computed_tag_poses[i].z = tag_stats[0][2];

        computed_tag_poses[i].roll = tag_stats[2][0];
        computed_tag_poses[i].pitch = tag_stats[2][1];
        computed_tag_poses[i].yaw = tag_stats[2][2];

//        std::cout << "Tag " << computed_tag_poses[i].tag_id
//            << "\n\tmean"
//            << "\n\t\txyz: " << tag_stats[0][0] << ", " << tag_stats[0][1] << ", " << tag_stats[0][2]
//            << "\n\t\trpy: " << tag_stats[2][0] << ", " << tag_stats[2][1] << ", " << tag_stats[2][2]
//            << std::endl
//            << "\n\tstandard deviation"
//            << "\n\t\txyz: " << tag_stats[1][0] << ", " << tag_stats[1][1] << ", " << tag_stats[1][2]
//            << "\n\t\trpy: " << tag_stats[3][0] << ", " << tag_stats[3][1] << ", " << tag_stats[3][2]
//            << std::endl;


    }

    return computed_tag_poses;
}