#pragma once

#include "MatrixHelpers.h"
#include "Pose.h"
#include "opencv2/opencv.hpp"

using namespace Localization;

inline std::vector<int> GetAttachedCamIDs(int max_index){
    std::vector<int> cam_ids;
    for (int i=0; i<max_index; i++){
        cv::VideoCapture cap(i);
        if (!cap.isOpened()) continue;
        cam_ids.push_back(i);
    }
    return cam_ids;
}

class TagDetectorCamera{
public:
    /***
     * Setup the AprilTag detector & CV2 camera capture logic. Once TagDetectorCamera object per camera
     * @param getopt
     * @param camera
     * @param R_camera_robot
     * @param t_camera_robot
     */
    TagDetectorCamera(getopt_t* getopt, int camera_id, const Eigen::Matrix3d& R_camera_robot, const Eigen::Vector3d& t_camera_robot);

    cv::Mat GetImage();

    TagArray GetTagsFromImage(const cv::Mat& img);

    cv::Mat DrawTagBoxesOnImage(const TagArray& tags, const cv::Mat& img);

    void ImShow(const std::string& title, int timeout, const cv::Mat& img);

protected:
    getopt_t* _opts;
    int _camera_id;
    Eigen::Matrix3d _R_camera_robot;
    Eigen::Vector3d _T_camera_robot;

    cv::VideoCapture _cap;
    apriltag_detector_t* _tag_detector;


};