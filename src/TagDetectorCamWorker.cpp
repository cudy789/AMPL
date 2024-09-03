#include "TagDetectorCamWorker.h"

TagDetectorCamWorker::TagDetectorCamWorker(getopt_t *getopt, int cameraId, const Eigen::Matrix3d &rCameraRobot,
                                           const Eigen::Vector3d &tCameraRobot, std::function<bool(TagArray&)> queue_tags_callback)
                                           : TagDetectorCamera(getopt, cameraId, rCameraRobot, tCameraRobot) {
    _show_im = true;
    _queue_tags_callback = queue_tags_callback;

}

void TagDetectorCamWorker::Start() {
    if (!_cap.isOpened()){
        std::cerr << "Camera " << _camera_id << " cannot be opened, will not start detector" << std::endl;
    } else{
        std::cout << "Starting Tag Detector thread for cam " << _camera_id << std::endl;
        _t_worker = std::thread([this]() {this->Run();});
    }
}

void TagDetectorCamWorker::join(){
    if (!_cap.isOpened()) return;
    _t_worker.join();
}

void TagDetectorCamWorker::Run() {
    std::vector<ulong> runtimes;
    std::cout << "Starting run function for camera " << _camera_id << std::endl;

    while (true){
        ulong start_ns = CurrentTime();

        cv::Mat img = GetImage();
        if (!img.empty()){
            TagArray raw_tags = GetTagsFromImage(img);

            bool success = _queue_tags_callback(raw_tags);
            if (!success){
                std::cerr << "ERROR Camera " << _camera_id << ": error acquiring lock to add tags to processing queue" << std::endl;
            }
//            cv::Mat tags_img = DrawTagBoxesOnImage(raw_tags, img);
//
//            ImShow("Camera " + std::to_string(_camera_id) + " img", 1, tags_img);

            runtimes.push_back(CurrentTime() - start_ns);
            if (runtimes.size() == 50){
                std::cout << "Average camera " << _camera_id << " worker execution time: " << (std::accumulate(runtimes.begin(), runtimes.end(), 0.0) / (1.0e6 * runtimes.size())) << "ms" << std::endl;
                std::cout << "\t Max: " << *std::max_element(runtimes.begin(), runtimes.end()) / 1.0e6 << "ms" << std::endl;
                runtimes.clear();
            }
        }
//        std::this_thread::yield();


    }


}