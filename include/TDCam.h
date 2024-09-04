#pragma once

#include "MatrixHelpers.h"
#include "Pose.h"
#include "opencv2/opencv.hpp"
#include "Logger.h"

extern "C" {
#include "apriltag.h"
#include "tag36h11.h"
#include "tag25h9.h"
#include "tag16h5.h"
#include "tagCircle21h7.h"
#include "tagCircle49h12.h"
#include "tagCustom48h12.h"
#include "tagStandard41h12.h"
#include "tagStandard52h13.h"
#include "common/getopt.h"
}

inline std::vector<int> GetAttachedCamIDs(int max_index){
    std::vector<int> cam_ids;
    for (int i=0; i<max_index; i++){
        cv::VideoCapture cap(i);
        if (!cap.isOpened()) continue;
        cam_ids.push_back(i);
    }
    return cam_ids;
}

struct CamParams{
    int camera_id;
    Eigen::Matrix3d R_camera_robot;
    Eigen::Vector3d T_camera_robot;

    struct {
        float quad_decimate = 2.0; // Decimate input image by this factor
        float quad_sigma = 0.0; // Apply low-pass blur to input
        int nthreads = 1; // Use this many CPU threads for AT detection
        bool debug = false; // Enable debugging output (slow)
        bool refine_edges = true; // Spend more time trying to align edges of tags
    } tag_detector;
};

class TDCam{
public:
    /***
     * Setup the AprilTag detector & CV2 camera capture logic. Once TagDetectorCamera object per camera
     * @param getopt
     * @param camera
     * @param R_camera_robot
     * @param t_camera_robot
     */
    TDCam() = delete;

    explicit TDCam(CamParams& c_params);

    virtual ~TDCam();

    cv::Mat GetImage();

    TagArray GetTagsFromImage(const cv::Mat& img);

    cv::Mat DrawTagBoxesOnImage(const TagArray& tags, const cv::Mat& img);

    void ImShow(const std::string& title, int timeout, const cv::Mat& img);

protected:
    CamParams _c_params;

    cv::VideoCapture _cap;

    apriltag_detector_t* _tag_detector;
    apriltag_family_t* _tf;


};