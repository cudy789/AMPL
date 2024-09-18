#include <cmath>

#include "Localization.h"


std::vector<std::vector<double>> Localization::Stats(std::vector<TagPose>& tag_poses){
    int count = tag_poses.size();

    std::vector<double> mean_xyz = {0,0,0};
    std::vector<double> mean_rpy = {0,0,0};
    std::vector<double> std_dev_xyz = {0,0,0};
    std::vector<double> std_dev_rpy = {0,0,0};

    if (count > 0) {

        // Calculate mean
        for (TagPose& p: tag_poses) {
            mean_xyz[0] += p.T[0];
            mean_xyz[1] += p.T[1];
            mean_xyz[2] += p.T[2];

            Eigen::Vector3d rpy = RotationMatrixToRPY(p.R);
            mean_rpy[0] += rpy[0];
            mean_rpy[1] += rpy[1];
            mean_rpy[2] += rpy[2];
        }
        for (double &v: mean_xyz) {
            v /= count;
        }
        for (double &v: mean_rpy) {
            v /= count;
        }

        // Calculate variance = SUM(x - x_mean)^2

        for (TagPose p: tag_poses) {
            std_dev_xyz[0] += (p.T[0] - mean_xyz[0]) * (p.T[0] - mean_xyz[0]);
            std_dev_xyz[1] += (p.T[1] - mean_xyz[1]) * (p.T[1] - mean_xyz[1]);
            std_dev_xyz[2] += (p.T[2] - mean_xyz[2]) * (p.T[2] - mean_xyz[2]);

            Eigen::Vector3d rpy = RotationMatrixToRPY(p.R);
            std_dev_rpy[0] += (rpy[0] - mean_rpy[0]) * (rpy[0] - mean_rpy[0]);
            std_dev_rpy[1] += (rpy[1] - mean_rpy[1]) * (rpy[1] - mean_rpy[1]);
            std_dev_rpy[2] += (rpy[2] - mean_rpy[2]) * (rpy[2] - mean_rpy[2]);
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

// TODO make this a strategy design pattern
// Particle filter based on previous pose(s) and dt
// Kalman filter with covariance based on tag ambiguity, distance, how many cameras see the same tag

std::vector<TagPose> Localization::DisambiguateTags(TagArray& fresh_tag_poses){
    std::vector<TagPose> computed_tag_poses{NUM_TAG_IDS};

    // Just average out the values of multiple poses. TODO do something smarter!
    for (int i = 0; i < NUM_TAG_IDS; i++) {
        std::vector<TagPose> &r_v = fresh_tag_poses.data[i];

        if (r_v.empty()) continue;

        std::vector<std::vector<double>> tag_stats = Stats(r_v);

        computed_tag_poses[i].tag_id = r_v[0].tag_id;

        computed_tag_poses[i].T[0] = tag_stats[0][0];
        computed_tag_poses[i].T[1] = tag_stats[0][1];
        computed_tag_poses[i].T[2] = tag_stats[0][2];

        Eigen::Matrix3d new_R = CreateRotationMatrix({tag_stats[2][0], tag_stats[2][1], tag_stats[2][2]});
        computed_tag_poses[i].R = new_R;

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