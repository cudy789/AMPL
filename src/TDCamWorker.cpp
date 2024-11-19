#include "TDCamWorker.h"

TDCamWorker::TDCamWorker(CamParams& c_params, const std::map<int, Pose_single>& tag_layout,
                         std::function<bool(TagArray&)> queue_tags_callback, bool record_video)
        : Worker{"TDCamWorker " + c_params.name, true, 50},
          TDCam{c_params, tag_layout, record_video},
          _queue_tags_callback{std::move(queue_tags_callback)}
          {}

void TDCamWorker::Init() {
    InitCap();
    InitDetector();

    if (!_cap.isOpened()){
        AppLogger::Logger::Log("Camera " + std::to_string(_c_params.camera_id) + " cannot be opened", AppLogger::SEVERITY::ERROR);
        Stop(false);
        sleep(5);

    } else{
        AppLogger::Logger::Log("Starting tag detector for cam " + std::to_string(_c_params.camera_id));
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
                if (!_last_img.empty()){
                    // We "dropped" a frame, so add more frames to keep up with specified framerate
                    _leftover_save_time += CurrentTime() - _last_save_time;
                    if (_leftover_save_time < (1.0e9/_c_params.fps)) _leftover_save_time = 0;
                    else _leftover_save_time -= 1.0e9/_c_params.fps;

                    int extra_frames = (int)std::round((double)_leftover_save_time / (1.0e9/_c_params.fps));

                    if (extra_frames > 0) {
                        _leftover_save_time = 0;
                    }
                    for (int i=0; i<extra_frames; i++){
                        SaveImage(_last_img);
                    }
                }
                SaveImage(img);
                _last_save_time = CurrentTime();
            }
            _last_img = img.clone();



            TagArray raw_tags = GetTagsFromImage(img);

            cv::Mat box_img = DrawTagBoxesOnImage(raw_tags, img);

            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << GetExecutionFreq();

            // Put fps in top right corner
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
        AppLogger::Logger::Log("Error getting img from camera " + _c_params.name, AppLogger::SEVERITY::WARNING);
        Stop(false);
        sleep(5);
    }

}