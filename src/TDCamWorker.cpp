#include "TDCamWorker.h"

TDCamWorker::TDCamWorker(CamParams& c_params, const std::map<int, Pose_single>& tag_layout,
                         std::function<bool(TagArray&)> queue_tags_callback, bool record_video)
        : Worker{"TDCamWorker " + c_params.name, true, 50},
          TDCam{c_params, tag_layout, record_video},
          _queue_tags_callback{std::move(queue_tags_callback)}
          {}

void TDCamWorker::Init() {
    if (!_c_params.camera_playback_file.empty()){
        InitRecordedCap();
        SetExecutionFreq(_c_params.fps);
    } else{
        InitCap();
    }
    InitDetector();

    if (!_cap.isOpened()){
        AppLogger::Logger::Log("Camera " + std::to_string(_c_params.camera_id) + " cannot be opened", AppLogger::SEVERITY::ERROR);
        Stop(false);
        sleep(5);

    } else{
        AppLogger::Logger::Log("Starting tag detector for cam " + std::to_string(_c_params.camera_id));
        if (_camera_matrix.empty()){
            AppLogger::Logger::Log("No distortion matrix was found, not undistorting images before processing", AppLogger::SEVERITY::WARNING);
        }
    }
}

void TDCamWorker::Finish() {
    CloseCap();
}

cv::Mat TDCamWorker::GetAnnotatedIm() {
    cv::Mat ret_mat;
    if (_annotated_im_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(500))){
        ret_mat = _annotated_im.clone();
        _annotated_im_sem.release();
    }
    return ret_mat;
}

void TDCamWorker::Execute() {
    cv::Mat img = GetImage();

    if (!img.empty()) {
        try {
            // Save image to video file
            if (_enable_video_writer){
                if (_start_time == 0) _start_time = CurrentTime();
                SaveImage(img);

                auto period_time = [this]() -> long {return (CurrentTime() - _start_time) % (ulong)1.0e9;};
                _period_frames_saved++;

                // Check if we need to save another frame this period, the camera might be running at a lower FPS than
                // what is specified. We must maintain a constant framerate when writing to a video file.
                if (_period_frames_saved * _ns_per_frame < period_time()){

                    int extra_frames = std::ceil((period_time() - (_period_frames_saved * _ns_per_frame)) / _ns_per_frame);
                    AppLogger::Logger::Log(_c_params.name + " needs " + to_string(extra_frames) + " extra frames", AppLogger::SEVERITY::DEBUG);
                    for (int i=0; i<extra_frames; i++){
                        SaveImage(img);
                        _period_frames_saved++;
                    }
                }
                if (_period_frames_saved > _c_params.fps) _period_frames_saved=0;
            }

            // Undistort the image
            if (!_camera_matrix.empty()){
                Undistort(img);
            }

            // Find tags in image
            TagArray raw_tags = GetTagsFromImage(img);

            // Draw tags onto image
            cv::Mat box_img = DrawTagBoxesOnImage(raw_tags, img);

            // Put fps in top right corner
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << GetExecutionFreq();
            cv::putText(box_img, ss.str(), cv::Point(box_img.size().width - 50, 0 + 20),
                        cv::FONT_HERSHEY_DUPLEX, 0.65, cv::Scalar(0, 255, 0), 2);

            // Put camera name in top left corner
            cv::putText(box_img, _c_params.name, cv::Point(10, 0 + 20),
                        cv::FONT_HERSHEY_DUPLEX, 0.65, cv::Scalar(0, 255, 0), 2);

            // Update latest annotated image
            if (_annotated_im_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))) {
                _annotated_im = box_img;
                _annotated_im_sem.release();
            }

            bool success = _queue_tags_callback(raw_tags);
            if (!success) {
                AppLogger::Logger::Log("Camera " + std::to_string(_c_params.camera_id) +
                                       " error acquiring lock to add tags to processing queue",
                                       AppLogger::SEVERITY::WARNING);
            }
        } catch(cv::Exception& e)
        {
            AppLogger::Logger::Log(e.what(), AppLogger::SEVERITY::ERROR);
        }
    } else {
        if (!_c_params.camera_playback_file.empty()){
            if (errno != EAGAIN){
                AppLogger::Logger::Log("Reached the end of the video file " + _c_params.camera_playback_file + ", stopping thread.");
                Stop();
            }
        } else{
            AppLogger::Logger::Log("Error getting img from camera " + _c_params.name, AppLogger::SEVERITY::WARNING);
            Stop(false);
            sleep(5);
        }
    }

}