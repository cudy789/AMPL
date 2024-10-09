#include <cmath>

#include "Localization.h"
#include "Logger.h"

bool MeanLocalizationStrategy::Compute(TagArray &fresh_poses, RobotPose &filtered_pose) {
    // Compute the mean of the global poses for each tag
    Pose_single avg_filtered_pose;
    int num_poses = 0;


    // Compute average
    // for each tag in a given camera, only select the tag pose with the least error
    for (std::vector<Pose>& v: fresh_poses.data){
        std::vector<Pose> least_error_tags;

        // count number of unique cameras that have detected this tag
        std::map<int, Pose*> cam_pose_err;
        for (Pose& p: v){
            if (cam_pose_err.find(p.cam_id) == cam_pose_err.end()){
                cam_pose_err[p.cam_id] = &p;
            }
        }

        // find the max error for each camera
        for (Pose& p: v){
            if (p.err < cam_pose_err[p.cam_id]->err){
                cam_pose_err[p.cam_id] = &p;
            }
        }

        // add maxes to average
        for (const auto& pair: cam_pose_err){
            avg_filtered_pose += pair.second->global;
            num_poses++;
        }

    }

//    AppLogger::Logger::Log("num_poses used to calculate average in localization strategy: " + std::to_string(num_poses));
    if (num_poses > 0) {
        avg_filtered_pose /= num_poses;
    } else{
        filtered_pose.global = avg_filtered_pose;
        return false;
    }


    filtered_pose.global = avg_filtered_pose;

    return true;
}

// TODO this is not handling noise rejection well, probably due to few data points. We should try to weight the clusters based on the error of the poses within them (?)
bool KMeansLocalizationStrategy::Compute(TagArray &fresh_poses, RobotPose &filtered_pose) {
    Pose_single kmeans_pose;
    // kmeans clustering
    int k_clusters = 3;

    if (fresh_poses._num_tags > 0){
        std::vector<std::array<double, 6>> data;

        for (std::vector<Pose>& v: fresh_poses.data){
            for (Pose& p: v){
                Eigen::Vector3d rpy = RotationMatrixToRPY(p.global.R);

                data.push_back({p.global.T[0], p.global.T[1], p.global.T[2], rpy[0], rpy[1], rpy[2]}); // TODO this is not going to work with the rpy wrapping from 180 -> -180
            }
        }

        auto clustered_data = dkm::kmeans_lloyd(data, k_clusters);

        // Debugging statements
        AppLogger::Logger::Log("Means:");
        for (const auto& mean : std::get<0>(clustered_data)) {
            AppLogger::Logger::Log("\t(" + to_string(mean[0]) + "," + to_string(mean[1]) + "," + to_string(mean[2]) +
                                         "," + to_string(mean[3]) + "," + to_string(mean[4]) + "," + to_string(mean[5]) +")");
        }
//        AppLogger::Logger::Log("Cluster labels:");
//        AppLogger::Logger::Log("\tPoint:");
//        std::stringstream value;
//        for (const auto& point : data) {
//            value << "(" << point[0] << "," << point[1] << ") ";
//        }
//        AppLogger::Logger::Log(value.str());
//
//        AppLogger::Logger::Log("\tLabel:");
//        for (const auto& label : std::get<1>(clustered_data)) {
//            AppLogger::Logger::Log(to_string(label));
//        }

        // find the cluster with the most values in it
        std::vector<int> num_labels(k_clusters, 0);
        for (const auto& label : std::get<1>(clustered_data)) {
            num_labels[label]++;
        }
        auto max_it = std::max_element(num_labels.begin(), num_labels.end());
        int max_k = std::distance(num_labels.begin(), max_it);
        AppLogger::Logger::Log("max_k=" + to_string(max_k));

        // get the mean of the cluster
        std::array<double, 6> cluster_center = std::get<0>(clustered_data)[max_k];
        kmeans_pose.T = Eigen::Vector3d{cluster_center[0], cluster_center[1], cluster_center[2]};
        kmeans_pose.R = CreateRotationMatrix(Eigen::Vector3d{cluster_center[3], cluster_center[4], cluster_center[5]});

        // set the mean of the cluster as the new pose
        filtered_pose.global = kmeans_pose;
        return true;
    }

    filtered_pose.global = kmeans_pose;
    return true;
}