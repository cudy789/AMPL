#include "TagDetectorCamera.h"

TagDetectorCamera::TagDetectorCamera(getopt_t *getopt, int camera_id, const Eigen::Matrix3d &R_camera_robot,
                                     const Eigen::Vector3d &T_camera_robot) {
    _opts = getopt;
    _camera_id = camera_id;
    _R_camera_robot = R_camera_robot;
    _T_camera_robot = T_camera_robot;

    std::cout << "Enabling video capture of camera " << _camera_id << std::endl;

    ulong start_ns = Localization::CurrentTime();


    // Initialize camera
    _cap = cv::VideoCapture (_camera_id);
    if (!_cap.isOpened()) {
        std::cerr << "Couldn't open video capture device " << _camera_id << std::endl;
        return;
    }

    // Initialize tag detector with options
    apriltag_family_t *tf = NULL;
    const char *famname = getopt_get_string(getopt, "family");
    if (!strcmp(famname, "tag36h11")) {
        tf = tag36h11_create();
    } else if (!strcmp(famname, "tag25h9")) {
        tf = tag25h9_create();
    } else if (!strcmp(famname, "tag16h5")) {
        tf = tag16h5_create();
    } else if (!strcmp(famname, "tagCircle21h7")) {
        tf = tagCircle21h7_create();
    } else if (!strcmp(famname, "tagCircle49h12")) {
        tf = tagCircle49h12_create();
    } else if (!strcmp(famname, "tagStandard41h12")) {
        tf = tagStandard41h12_create();
    } else if (!strcmp(famname, "tagStandard52h13")) {
        tf = tagStandard52h13_create();
    } else if (!strcmp(famname, "tagCustom48h12")) {
        tf = tagCustom48h12_create();
    } else {
        printf("Unrecognized tag family name. Use e.g. \"tag36h11\".\n");
        exit(-1);
    }


    // Create tag detector for specific tag family
    _tag_detector = apriltag_detector_create();
    apriltag_detector_add_family(_tag_detector, tf);

    if (errno == ENOMEM) {
        std::cout << "Unable to add family to detector due to insufficient memory to allocate the tag-family decoder "
                     "with the default maximum hamming value of 2. Try choosing an alternative tag family." << std::endl;
        exit(-1);
    }

    // Set detector options
    _tag_detector->quad_decimate = getopt_get_double(_opts, "decimate");
    _tag_detector->quad_sigma = getopt_get_double(_opts, "blur");
    _tag_detector->nthreads = getopt_get_int(_opts, "threads");
    _tag_detector->debug = getopt_get_bool(_opts, "debug");
    _tag_detector->refine_edges = getopt_get_bool(_opts, "refine-edges");

    ulong end_ns = Localization::CurrentTime();
    std::cout << "Camera AprilTag detector " << famname << " initialized in "
         << std::fixed << std::setprecision(3) << (end_ns - start_ns) / 1.0e9 << " seconds" << std::endl;

    std::cout << "  " << _cap.get(cv::CAP_PROP_FRAME_WIDTH ) << "x" <<
         _cap.get(cv::CAP_PROP_FRAME_HEIGHT ) << " @" <<
         _cap.get(cv::CAP_PROP_FPS) << "FPS" << std::endl;

}

cv::Mat TagDetectorCamera::GetImage() {
    cv::Mat img;
    _cap >> img;
    if (img.empty()){
        std::cerr << "error getting img from camera " << _camera_id << ", resetting capture\n";
        sleep(1);
        _cap = cv::VideoCapture(_camera_id);
        return cv::Mat();
    }

    return img;
}

TagArray TagDetectorCamera::GetTagsFromImage(const cv::Mat &img) {
    ulong start_ns = CurrentTime();

    TagArray detected_tags;

    // Convert img to grayscale
    cv::Mat gray;
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Make an image_u8_t header for the Mat data
    image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};

    // Detect tags in image
    zarray_t *detections = apriltag_detector_detect(_tag_detector, &im);
    if (errno == EAGAIN) {
        printf("Unable to create the %d threads requested.\n", _tag_detector->nthreads);
        exit(-1);
    }

    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);

        // Calculate pose of tag
        apriltag_detection_info_t info;
        info.det = det;
//            info.tagsize = 0.165; // in meters, TODO good guess, but need actual size
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

        // Rotate AprilTag from the tag frame into the robot's coordinate frame
        Eigen::Matrix3d R_tag_robot = _R_camera_robot * R_tag_camera;
        Eigen::Vector3d R_tag_robot_rpy = RotationMatrixToRPY(R_tag_robot);

        // Translate AprilTag from the tag frame into the robot's coordinate frame
        Eigen::Vector3d T_tag_robot = _R_camera_robot * T_tag_camera + _T_camera_robot;

        // Debug output
//        std::cout << "Detection on camera " << _camera_id << std::endl;
//        std::cout << "\ttag ID: " << det->id << std::endl;
//        std::cout << "\tpose xyz in robot frame: " << T_tag_robot << std::endl;
//        std::cout << "\tpose rpy in robot frame: " << R_tag_robot_rpy << std::endl;

        // Add tag to detected TagArray object
        detected_tags.data[det->id-1].push_back(TagPose{Pose{pose.t->data[0], pose.t->data[1], pose.t->data[2],
                                                             R_tag_robot_rpy[0], R_tag_robot_rpy[1], R_tag_robot_rpy[2]},
                                                        det->id, _camera_id,
                                                        det->c[0], det->c[1],
                                                        det->p[0][0], det->p[0][1],
                                                        det->p[1][0], det->p[1][1],
                                                        det->p[2][0], det->p[2][1],
                                                        det->p[3][0], det->p[3][1]
        });
    }

    ulong end_ns = CurrentTime();
    apriltag_detections_destroy(detections);
//    std::cout << "Processed AprilTags in img for camera " << _camera_id << " in " << (end_ns - start_ns) / 1.0e6 << "ms" << std::endl;

    return detected_tags;
}

cv::Mat TagDetectorCamera::DrawTagBoxesOnImage(const TagArray &tags, const cv::Mat &img) {
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

void TagDetectorCamera::ImShow(const std::string& title, int timeout, const cv::Mat& img) {
    imshow(title, img);
    if (cv::waitKey(timeout) >= 0) return;
}