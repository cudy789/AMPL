#include "CalibrationCamWorker.h"

CalibrationCamWorker::CalibrationCamWorker(CamParams& c_params)
        : Worker{"CalibrationCamWorker " + c_params.name, true, 50},
          TDCam{c_params, {}, false}
          {}

void CalibrationCamWorker::Init() {
    InitCap();

    if (!_cap.isOpened()){
        AppLogger::Logger::Log("Camera " + std::to_string(_c_params.camera_id) + " cannot be opened", AppLogger::SEVERITY::ERROR);
        Stop(false);
        sleep(5);
    } else{
        AppLogger::Logger::Log("Starting calibration for cam " + std::to_string(_c_params.camera_id));
        AppLogger::Logger::Log("Make sure checkerboard is in the camera frame. Slowly move the checkerboard around the camera frame during calibration. Beginning in 10 seconds");

        // Prepare object points, like (0,0,0), (1,0,0), (2,0,0), ..., (8,5,0)
        for (int i = 0; i < _pattern_size.height; ++i) {
            for (int j = 0; j < _pattern_size.width; ++j) {
                _objp.push_back(cv::Point3f(j, i, 0));
            }
        }


    }
}

cv::Mat CalibrationCamWorker::GetAnnotatedIm() {
    cv::Mat ret_mat;
    if (_annotated_im_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(500))){
        ret_mat = _annotated_im.clone();
        _annotated_im_sem.release();
    }
    return ret_mat;
}

void CalibrationCamWorker::Finish() {
    CloseCap();
}

void CalibrationCamWorker::Execute() {

    cv::Mat img = GetImage();

    if (current_calibration_images == target_calibration_images) {
        int flags = 0;
        cv::Mat initial_camera_matrix, camera_matrix, dist_coeffs;
        std::vector<cv::Mat> rvecs, tvecs;

        if (!_camera_matrix.empty()){
            AppLogger::Logger::Log("Using the provided values for fx, fy, cx, cy as an initial guess");
            initial_camera_matrix = _camera_matrix;
            flags = cv::CALIB_USE_INTRINSIC_GUESS;
        }

        // Calculate the intrinsic camera matrix and distortion coefficients
        cv::Size image_size(img.cols, img.rows); // Reverse of `gray.shape[::-1]` in Python
        bool ret = cv::calibrateCamera(
                _obj_points,
                _img_points,
                image_size,
                initial_camera_matrix,
                dist_coeffs,
                rvecs,
                tvecs,
                flags
        );

        camera_matrix = cv::getOptimalNewCameraMatrix(
                initial_camera_matrix,
                dist_coeffs,
                image_size,
                1,
                image_size
        );

        if (ret){
            double fx = camera_matrix.at<cv::Vec3d>(0)[0];
            double fy = camera_matrix.at<cv::Vec3d>(1)[1];
            double cx = camera_matrix.at<cv::Vec3d>(0)[2];
            double cy = camera_matrix.at<cv::Vec3d>(1)[2];

            double k1 = dist_coeffs.at<cv::Vec3d>(0)[0];
            double k2 = dist_coeffs.at<cv::Vec3d>(0)[1];
            double k3 = dist_coeffs.at<cv::Vec3d>(0)[4];
            double p1 = dist_coeffs.at<cv::Vec3d>(0)[2];
            double p2 = dist_coeffs.at<cv::Vec3d>(0)[3];

            AppLogger::Logger::Log("Successfully calibrated camera " + _c_params.name);
            std::string s_params =
                    "\n########################################\n"
                    + _c_params.name + " results\n"
                    "\tCopy and paste the following into your config.yaml file under " + _c_params.name + " to apply the calibration:\n\n"
                    "\tfx: " + to_string(fx) + "\n"
                    "\tfy: " + to_string(fy) + "\n"
                    "\tcx: " + to_string(cx) + "\n"
                    "\tcy: " + to_string(cy) + "\n"
                    "\tdist_coeffs: [" + to_string(k1) + ", " + to_string(k2) + ", " + to_string(p1) +
                    ", " + to_string(p2) + ", " + to_string(k3) + "]\n\n"
                    "########################################";

            AppLogger::Logger::Log(s_params);
            Stop(true);
        } else{
            AppLogger::Logger::Log("Could not calculate camera distortion coefficients, retrying...");
            Stop();
        }
        return;
    }

    if (!img.empty()) {
        try {

            bool pattern_found = false;

            if (initial_delay_counter == 0) initial_delay_counter = CurrentTime();
            if ((CurrentTime() - initial_delay_counter) / 1.0e9 > initial_delay) {
                if (calibration_loop_start == 0) calibration_loop_start = CurrentTime();

                // Calculate the distortion coefficients from the current image
                if ((CurrentTime() - calibration_loop_start) / 1.0e9 > calibration_loop_duration) {
                    calibration_loop_start = CurrentTime();

                    cv::Mat gray;
                    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

                    std::vector<cv::Point2f> corners;

                    pattern_found = cv::findChessboardCorners(gray, _pattern_size, corners,
                                                              cv::CALIB_CB_ADAPTIVE_THRESH +
                                                              cv::CALIB_CB_NORMALIZE_IMAGE +
                                                              cv::CALIB_CB_FAST_CHECK);

                    if (pattern_found) {
                        _obj_points.push_back(_objp);
                        _img_points.push_back(corners);

                        cornerSubPix(gray, corners, {11, 11}, {-1, -1},
                                     cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
                        drawChessboardCorners(img, _pattern_size, cv::Mat(corners), pattern_found);
                        current_calibration_images++;
                    }

                }
            }


            // Put fps in top right corner
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << GetExecutionFreq();
            cv::putText(img, ss.str(), cv::Point(img.size().width - 50, 0 + 20),
                        cv::FONT_HERSHEY_DUPLEX, 0.65, cv::Scalar(0, 255, 0), 2);

            // Put camera name in top left corner
            cv::putText(img, _c_params.name + " [CALIBRATING " + to_string(current_calibration_images) + "/" + to_string(target_calibration_images) + "]",
                        cv::Point(10, 0 + 20),cv::FONT_HERSHEY_DUPLEX, 0.65, cv::Scalar(0, 255, 0), 2);

            // Update latest annotated image
            if (_annotated_im_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))) {
                _annotated_im = img;
                _annotated_im_sem.release();
            }

            if (pattern_found) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250)); // display the image on the webgui for longer
            }

        } catch(cv::Exception& e) {
            AppLogger::Logger::Log(e.what(), AppLogger::SEVERITY::ERROR);
        }
    } else {
        AppLogger::Logger::Log("Error getting img from camera " + _c_params.name, AppLogger::SEVERITY::WARNING);
        Stop(false);
        sleep(5);
    }

}