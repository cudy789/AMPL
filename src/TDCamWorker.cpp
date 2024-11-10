#include "TDCamWorker.h"

TDCamWorker::TDCamWorker(CamParams& c_params, std::function<bool(TagArray&)> queue_tags_callback, bool show_im)
        : Worker{"TDCamWorker " + c_params.name, true, 50},  // Call Worker constructor
          TDCam{c_params},  // Call TDCam constructor
          _queue_tags_callback{std::move(queue_tags_callback)},
          _show_im{show_im} {

}

void TDCamWorker::Init() {
    InitCap();

    if (!_cap.isOpened()){
        AppLogger::Logger::Log("Camera " + std::to_string(_c_params.camera_id) + " cannot be opened", AppLogger::SEVERITY::ERROR);

    } else{
        AppLogger::Logger::Log("Starting tag detector for cam " + std::to_string(_c_params.camera_id));
    }
}

void TDCamWorker::Finish() {
    CloseCap();
}

cv::Mat TDCamWorker::GetAnnotatedIm() {
    if (_annotated_im_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(500))){
        _annotated_im_sem.release();
        return _annotated_im;
    }
    return cv::Mat();
}

void TDCamWorker::Execute() {

    ulong start_ns = CurrentTime();
    cv::Mat img = GetImage();
    if (!img.empty()) {
        try {
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


            if (_annotated_im_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))) {
                _annotated_im = box_img;
                _annotated_im_sem.release();
            }

            bool success = _queue_tags_callback(raw_tags);

            ulong duration_ns = CurrentTime() - start_ns;
            if (!success) {
                AppLogger::Logger::Log("Camera " + std::to_string(_c_params.camera_id) +
                                       " error acquiring lock to add tags to processing queue",
                                       AppLogger::SEVERITY::WARNING);
            }
            if (_show_im) {
                cv::Mat tags_img = DrawTagBoxesOnImage(raw_tags, img);
                ImShow("Camera " + std::to_string(_c_params.camera_id) + " img", 1, tags_img);
            }
        } catch(cv::Exception& e)
        {
            AppLogger::Logger::Log(e.what(), AppLogger::SEVERITY::ERROR);
        }
    }

}