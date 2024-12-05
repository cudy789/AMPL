#pragma once

#include <thread>
#include <bits/stdc++.h>

#include "TDCam.h"
#include "Worker.h"

/**
 * @brief The threaded tag detector camera worker periodically polls its capture device for new camera frames, detects
 * Apriltags in the frame, then passes the Apriltag pose detections to the LocalizationWorker class via a callback function.
 * This worker also provides access to an annotated image with bounding boxes and tag IDs displayed around detected Apriltags
 * in the latest camera frame.
 */
class CalibrationCamWorker: public Worker, public TDCam {
public:
    /**
     * @brief No default constructor.
     */
    CalibrationCamWorker() = delete;
    /**
     * @brief Call parent class destructors.
     */
    ~CalibrationCamWorker() = default;
    /**
     *
     */
    CalibrationCamWorker(CamParams& c_params);
    /**
     * @brief
     */
    cv::Mat GetAnnotatedIm();


protected:
    /**
     * @brief Setup the capture device and start the detector. If the capture device cannot be opened, restart the thread.
     */
    void Init() override;
    /**
     * @brief Get an image from the capture device, compute the tag poses in the image, annotate the image with the
     * tag bounding boxes, the camera name, and current FPS. Run callback to send tag pose data, and display image locally
     * if flag is set.
     */
    void Execute() override;
    /**
     * @brief Close the capture device.
     */
    void Finish() override;

    std::binary_semaphore _annotated_im_sem{1};
    cv::Mat _annotated_im;

    cv::Size _pattern_size{9,6}; // checkerboard pattern size
    std::vector<cv::Point3f> _objp;
    std::vector<std::vector<cv::Point3f>> _obj_points; // 3D points in real world space
    std::vector<std::vector<cv::Point2f>> _img_points; // 2D points in image plane


    double initial_delay = 10;
    ulong initial_delay_counter = 0;
    ulong calibration_loop_start = 0;
    double calibration_loop_duration = .5;
    int target_calibration_images = 20;
    int current_calibration_images = 0;


};