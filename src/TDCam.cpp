#include "TDCam.h"

TDCam::TDCam(CamParams& c_params) {
    _c_params = c_params;

    AppLogger::Logger::Log("Enabling video capture on camera " + _c_params.name);

    ulong start_ns = CurrentTime();


    // Initialize camera
    _cap = cv::VideoCapture (_c_params.camera_id, cv::CAP_V4L);
    if (!_cap.isOpened()) {
        AppLogger::Logger::Log("Enabling video capture on camera " +
            _c_params.name, AppLogger::SEVERITY::ERROR);
    }

//    cv::Mat temp;
//    _cap >> temp; // get first frame so we can adjust the settings

    _cap.set(cv::CAP_PROP_FPS, c_params.fps); // Frame rate
//    _cap.set(cv::CAP_PROP_FRAME_WIDTH, 544); // Width
//    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, 288); // Height
//    _cap.set(cv::CAP_PROP_FRAME_WIDTH, 640); // Width
//    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // Height
    _cap.set(cv::CAP_PROP_FRAME_WIDTH, c_params.res_x); // Width
    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, c_params.res_y); // Height

    _cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

//    _cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 3); // Turn off autoexposure = 1, on = 3
    _cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1); // Turn off autoexposure = 1, on = 3
    _cap.set(cv::CAP_PROP_EXPOSURE, c_params.exposure); // set exposure value, do not use auto exposure

    // Initialize tag detector with options
    _tf = tag36h11_create();

    // Create tag detector for specific tag family
    _tag_detector = apriltag_detector_create();
    apriltag_detector_add_family(_tag_detector, _tf);

    if (errno == ENOMEM) {
        AppLogger::Logger::Log("Unable to add family to detector due to insufficient memory to allocate the "
                               "tag-family decoder with the default maximum hamming value of 2. "
                               "Try choosing an alternative tag family.", AppLogger::SEVERITY::ERROR);
        exit(-1);
    }

    // Set detector options
    _tag_detector->quad_decimate = _c_params.tag_detector.quad_decimate;
    _tag_detector->quad_sigma = _c_params.tag_detector.quad_sigma;
    _tag_detector->nthreads = _c_params.tag_detector.nthreads;
    _tag_detector->debug = _c_params.tag_detector.debug;
    _tag_detector->refine_edges = _c_params.tag_detector.refine_edges;

    ulong end_ns = CurrentTime();
    AppLogger::Logger::Log("Camera AprilTag detector " + _c_params.name +
        " initialized in " + std::to_string((end_ns - start_ns) / 1.0e9) + " seconds" );

    AppLogger::Logger::Log(std::to_string(_cap.get(cv::CAP_PROP_FRAME_WIDTH)) + "x" +
        std::to_string(_cap.get(cv::CAP_PROP_FRAME_HEIGHT)) + " @" +
        std::to_string(_cap.get(cv::CAP_PROP_FPS)) + "FPS");
}

TDCam::~TDCam(){
    delete _tag_detector;
    delete _tf;
}

cv::Mat TDCam::GetImage() {
    cv::Mat img;
    _cap >> img;
    if (img.empty()){
        AppLogger::Logger::Log("Error getting img from camera " + _c_params.name, AppLogger::SEVERITY::WARNING);
//        _cap = cv::VideoCapture(_c_params.camera_id);
        sleep(2);
        return cv::Mat();
    }
    return img;
}

TagArray TDCam::GetTagsFromImage(const cv::Mat &img) {
    TagArray detected_tags;

    // Convert img to grayscale
    cv::Mat gray;
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Make an image_u8_t header for the Mat data
    image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};

    // Detect tags in image
    zarray_t *detections = apriltag_detector_detect(_tag_detector, &im);
    if (errno == EAGAIN) {
        AppLogger::Logger::Log("Unable to create the " + std::to_string(_tag_detector->nthreads) +
            " threads requested", AppLogger::SEVERITY::ERROR);
        exit(-1);
    }

    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);

        // Calculate pose of tag
        apriltag_detection_info_t info;
        info.det = det;
//            info.tagsize = 0.165; // in meters, TODO this is what it "should" be, but the distances don't work out
        info.tagsize = 0.095; // in meters, TODO hand tuned, probably not correct, scales okay with distance
        // Camera params from https://horus.readthedocs.io/en/release-0.2/source/scanner-components/camera.html
        info.fx = 1430;
        info.fy = 1430;

        info.cx = 320;
        info.cy = 240;

        // Calculate the tag's pose, return an error value as well
        apriltag_pose_t pose;
        double err = estimate_tag_pose(&info, &pose);

        // Construct matrices for the rotation (R) and translation (T) of the tag from the tag frame to the camera frame.
        // This is the rotation and position of the tag relative to the camera.
        Eigen::Matrix3d R_tag_camera = Array2EM<double, 3, 3>(pose.R->data);
        Eigen::Vector3d T_tag_camera = Array2EM<double, 3, 1>(pose.t->data);


        // ==================== AT -> Robot ====================
        // Rotate AprilTag from the tag frame into the robot's coordinate frame
        Eigen::Matrix3d R_tag_robot = _c_params.R_camera_robot * R_tag_camera;
        Eigen::Vector3d R_tag_robot_rpy = RotationMatrixToRPY(R_tag_robot);

        // Translate AprilTag from the tag frame into the robot's coordinate frame
        Eigen::Vector3d T_tag_robot = _c_params.R_camera_robot * T_tag_camera + _c_params.T_camera_robot;


        // ==================== Robot -> World ====================

        Pose& Pose_AG = TagLayout[det->id];

//        AppLogger::Logger::Log("Tag " + std::to_string(det->id) + " global rotation & translation: " + to_string(Pose_AG));
//        AppLogger::Logger::Log("Pose_AG.R: " + to_string(Pose_AG.R));

        Eigen::Vector3d T_robot_global = Pose_AG.R * ((-1.0 * T_tag_robot)) + Pose_AG.T;
        Eigen::Matrix3d R_robot_global = Pose_AG.R * R_tag_robot.transpose();

        TagPose world_tag{Pose{T_tag_robot, R_tag_robot},
                          det->id, _c_params.camera_id,
                          T_robot_global, R_robot_global,
                          err,
                          det->c[0], det->c[1],
                          det->p[0][0], det->p[0][1],
                          det->p[1][0], det->p[1][1],
                          det->p[2][0], det->p[2][1],
                          det->p[3][0], det->p[3][1]};


        // Add tag to detected TagArray object
        detected_tags.data[det->id-1].push_back(world_tag);
    }

//    ulong end_ns = CurrentTime();
    apriltag_detections_destroy(detections);
//    std::cout << "Processed AprilTags in img for camera " << _camera_id << " in " << (end_ns - start_ns) / 1.0e6 << "ms" << std::endl;

    return detected_tags;
}

cv::Mat TDCam::DrawTagBoxesOnImage(const TagArray &tags, const cv::Mat &img) {
    cv::Mat annotated_img = img;
    for (const std::vector<TagPose>& v: tags.data){
        for (const TagPose& p: v){

            // Draw detection outlines
            line(annotated_img, cv::Point(p.p[0][0], p.p[0][1]),
                 cv::Point(p.p[1][0], p.p[1][1]),
                 cv::Scalar(0, 0xff, 0), 2);
            line(annotated_img, cv::Point(p.p[0][0], p.p[0][1]),
                 cv::Point(p.p[3][0], p.p[3][1]),
                 cv::Scalar(0, 0, 0xff), 2);
            line(annotated_img, cv::Point(p.p[1][0], p.p[1][1]),
                 cv::Point(p.p[2][0], p.p[2][1]),
                 cv::Scalar(0xff, 0, 0), 2);
            line(annotated_img, cv::Point(p.p[2][0], p.p[2][1]),
                 cv::Point(p.p[3][0], p.p[3][1]),
                 cv::Scalar(0xff, 0, 0), 2);

            std::stringstream ss;
            ss << p.tag_id;
            cv::String text = ss.str();
            int fontface = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
            double fontscale = 1.0;
            int baseline;
            cv::Size textsize = cv::getTextSize(text, fontface, fontscale, 2,
                                        &baseline);
            putText(annotated_img, text, cv::Point(p.c[0]-textsize.width/2,
                                       p.c[1]+textsize.height/2),
                    fontface, fontscale, cv::Scalar(0xff, 0x99, 0), 2);
        }
    }
    return annotated_img;
}

void TDCam::ImShow(const std::string& title, int timeout, const cv::Mat& img) {
    imshow(title, img);
    if (cv::waitKey(timeout) >= 0) return;
}