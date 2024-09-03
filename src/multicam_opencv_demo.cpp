/* Copyright (C) 2013-2016, The Regents of The University of Michigan.
All rights reserved.
This software was developed in the APRIL Robotics Lab under the
direction of Edwin Olson, ebolson@umich.edu. This software may be
available under alternative licensing terms; contact the address above.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the Regents of The University of Michigan.
*/

#include <iomanip>
#include <thread>
#include <vector>
#include <cmath>
#include <unistd.h>

#include "MatrixHelpers.h"
#include "LocalizationWorker.h"
#include "TagDetectorCamWorker.h"

#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;



//vector<int> GetAttachedCamIDs(int max_index){
//    vector<int> cam_ids;
//    for (int i=0; i<max_index; i++){
//        VideoCapture cap(i);
//        if (!cap.isOpened()) continue;
//        cam_ids.push_back(i);
//    }
//    return cam_ids;
//}

//TODO move this to tag detector camera class
void ThreadDetect(getopt_t* getopt, int camera, const Eigen::Matrix3d& R_camera_robot, const Eigen::Vector3d& t_camera_robot){
    cout << "Enabling video capture of camera " << camera << endl;

    TickMeter meter;
    meter.start();

    // Initialize camera
    VideoCapture cap(camera);
    if (!cap.isOpened()) {
        cerr << "Couldn't open video capture device " << camera << endl;
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


    apriltag_detector_t *td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

    if (errno == ENOMEM) {
        printf("Unable to add family to detector due to insufficient memory to allocate the tag-family decoder with the default maximum hamming value of 2. Try choosing an alternative tag family.\n");
        exit(-1);
    }

    td->quad_decimate = getopt_get_double(getopt, "decimate");
    td->quad_sigma = getopt_get_double(getopt, "blur");
    td->nthreads = getopt_get_int(getopt, "threads");
    td->debug = getopt_get_bool(getopt, "debug");
    td->refine_edges = getopt_get_bool(getopt, "refine-edges");

    meter.stop();
    cout << "Detector " << famname << " initialized in "
         << std::fixed << std::setprecision(3) << meter.getTimeSec() << " seconds" << endl;
#if CV_MAJOR_VERSION > 3
    cout << "  " << cap.get(CAP_PROP_FRAME_WIDTH ) << "x" <<
                    cap.get(CAP_PROP_FRAME_HEIGHT ) << " @" <<
                    cap.get(CAP_PROP_FPS) << "FPS" << endl;
#else
    cout << "  " << cap.get(CV_CAP_PROP_FRAME_WIDTH ) << "x" <<
         cap.get(CV_CAP_PROP_FRAME_HEIGHT ) << " @" <<
         cap.get(CV_CAP_PROP_FPS) << "FPS" << endl;
#endif
    meter.reset();

    Mat frame, gray;
    while (true) {
        errno = 0;
        cap >> frame;
        if (frame.empty()){
            cerr << "error getting frame from camera " << camera << ", resetting capture\n";
            sleep(1);
            cap = VideoCapture(camera);
        }
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Make an image_u8_t header for the Mat data
        image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};

        zarray_t *detections = apriltag_detector_detect(td, &im);

        if (errno == EAGAIN) {
            printf("Unable to create the %d threads requested.\n",td->nthreads);
            exit(-1);
        }

        for (int i = 0; i < zarray_size(detections); i++) {
            apriltag_detection_t *det;
            zarray_get(detections, i, &det);


            // Calculate pose of tag
            apriltag_detection_info_t info;
            info.det = det;
//            info.tagsize = 0.165; // in meters, TODO good guess, but need actual size
            info.tagsize = 0.095; // in meters, TODO definitely not correct, does not scale well with distance
            // Camera params from https://horus.readthedocs.io/en/release-0.2/source/scanner-components/camera.html
            info.fx = 1430;
            info.fy = 1430;

            info.cx = 320;
            info.cy = 240;

            apriltag_pose_t pose;
            double err = estimate_tag_pose(&info, &pose);

            cout << "Detection on camera " << camera << endl;
            cout << "\ttag ID: " << det->id << endl;
            cout << "\tpose xyz: " << pose.t->data[0] << ", " << pose.t->data[1] << ", " << pose.t->data[2] << endl;
            Eigen::Vector3d rpy = RotationMatrixToRPY(pose.R);
            cout << "\tpose rpy: " << rpy << endl;
//            cout << "\terr: " << err << endl;
//            cout << "\tdecision_margin: " << det->decision_margin << endl;
//            cout << "\tfamily: " << det->family->name << endl;

            Eigen::Matrix3d R_tag_camera = Array2EM<double, 3, 3>(pose.R->data);

//            Eigen::Matrix3d R_tag_camera = Eigen::AngleAxisd(M_PI / 2, Eigen::Vector3d::UnitX()) * R_tag_camera_lhr;

            Eigen::Vector3d t_tag_camera = Array2EM<double, 3, 1>(pose.t->data);

            cout << "\tCamera rotation matrix: " << R_camera_robot << endl;
            Eigen::Matrix3d R_tag_robot = R_camera_robot * R_tag_camera;
            Eigen::Vector3d R_tag_robot_rpy = RotationMatrixToRPY(R_tag_robot);

            Eigen::Vector3d t_tag_robot = R_camera_robot * t_tag_camera + t_camera_robot;

            cout << "\n\n\tpose xyz in robot frame: " << t_tag_robot << endl;
            cout << "\tpose rpy in robot frame: " << R_tag_robot_rpy << endl;

//            Eigen::Vector3d gt(20, 30, 40);
//            Eigen::Matrix3d gt_r = CreateRotationMatrix(Deg2Rad(gt));




//            cout << "\n\n\ttest rpy2rot and back. Values should match: " << Eigen::Vector3d(20, 30, 40) << " == " << RotationMatrixToRPY(gt_r) << endl;


            // Draw detection outlines
            line(frame, Point(det->p[0][0], det->p[0][1]),
                 Point(det->p[1][0], det->p[1][1]),
                 Scalar(0, 0xff, 0), 2);
            line(frame, Point(det->p[0][0], det->p[0][1]),
                 Point(det->p[3][0], det->p[3][1]),
                 Scalar(0, 0, 0xff), 2);
            line(frame, Point(det->p[1][0], det->p[1][1]),
                 Point(det->p[2][0], det->p[2][1]),
                 Scalar(0xff, 0, 0), 2);
            line(frame, Point(det->p[2][0], det->p[2][1]),
                 Point(det->p[3][0], det->p[3][1]),
                 Scalar(0xff, 0, 0), 2);

            stringstream ss;
            ss << det->id;
            String text = ss.str();
            int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
            double fontscale = 1.0;
            int baseline;
            Size textsize = getTextSize(text, fontface, fontscale, 2,
                                        &baseline);
            putText(frame, text, Point(det->c[0]-textsize.width/2,
                                       det->c[1]+textsize.height/2),
                    fontface, fontscale, Scalar(0xff, 0x99, 0), 2);
        }
        apriltag_detections_destroy(detections);

        imshow("Tag Detections, cam " + to_string(camera), frame);
        if (waitKey(30) >= 0)
            break;
    }

    apriltag_detector_destroy(td);

    if (!strcmp(famname, "tag36h11")) {
        tag36h11_destroy(tf);
    } else if (!strcmp(famname, "tag25h9")) {
        tag25h9_destroy(tf);
    } else if (!strcmp(famname, "tag16h5")) {
        tag16h5_destroy(tf);
    } else if (!strcmp(famname, "tagCircle21h7")) {
        tagCircle21h7_destroy(tf);
    } else if (!strcmp(famname, "tagCircle49h12")) {
        tagCircle49h12_destroy(tf);
    } else if (!strcmp(famname, "tagStandard41h12")) {
        tagStandard41h12_destroy(tf);
    } else if (!strcmp(famname, "tagStandard52h13")) {
        tagStandard52h13_destroy(tf);
    } else if (!strcmp(famname, "tagCustom48h12")) {
        tagCustom48h12_destroy(tf);
    }

}


int main(int argc, char *argv[])
{
    getopt_t *getopt = getopt_create();

    getopt_add_bool(getopt, 'h', "help", 0, "Show this help");
    getopt_add_int(getopt, 'c', "camera", "0", "camera ID");
    getopt_add_bool(getopt, 'd', "debug", 0, "Enable debugging output (slow)");
    getopt_add_bool(getopt, 'q', "quiet", 0, "Reduce output");
    getopt_add_string(getopt, 'f', "family", "tag36h11", "Tag family to use");
    getopt_add_int(getopt, 't', "threads", "1", "Use this many CPU threads");
    getopt_add_double(getopt, 'x', "decimate", "2.0", "Decimate input image by this factor");
    getopt_add_double(getopt, 'b', "blur", "0.0", "Apply low-pass blur to input");
    getopt_add_bool(getopt, '0', "refine-edges", 1, "Spend more time trying to align edges of tags");

    if (!getopt_parse(getopt, argc, argv, 1) ||
            getopt_get_bool(getopt, "help")) {
        printf("Usage: %s [options]\n", argv[0]);
        getopt_do_usage(getopt);
        exit(0);
    }


//    ThreadWork(getopt, 0);
//    thread t_cam1(&ThreadWork, getopt, 0);
//    vector<int> cam_ids = GetAttachedCamIDs(15);
//    Eigen::Matrix3d R_camera_robot = CreateRotationMatrix({90, 0, 0});
//    Eigen::Vector3d t_camera_robot(0, 0, 0.55);

    vector<int> cam_ids = {2};
//    vector<thread> cam_threads;
    vector<TagDetectorCamWorker> cam_workers;
    LocalizationWorker l_worker;

    Eigen::Matrix3d rotmat;
    rotmat << 1, 0, 0,
              0, 0, 1,
              0, -1, 0;
                                                                    // roll, -90 is straight ahead
//    vector<Eigen::Matrix3d> R_camera_robots = {CreateRotationMatrix({-90, 0, 0})};
    vector<Eigen::Matrix3d> R_camera_robots = {
            CreateRotationMatrix({-90, 0, 25}), // 25 degrees w.r.t robot Z for right cam
            CreateRotationMatrix({-90, 0, 0}), // center cam
            CreateRotationMatrix({-90, 0, 0}) // center cam
    };



//    vector<Eigen::Matrix3d> R_camera_robots = {rotmat, rotmat};
//    vector<Eigen::Vector3d> t_camera_robots = {{0, 0, 0}}; // center cam
    vector<Eigen::Vector3d> t_camera_robots = {
            {0.14, -0.03, 0}, // right cam
            {0, 0, 0}, // center cam
            {0, 0, 0} // center cam
    };



    // Create tag detection threads
    for(int i=0; i<(int)cam_ids.size(); i++){
        cam_workers.emplace_back( TagDetectorCamWorker(getopt, cam_ids[i], R_camera_robots[i], t_camera_robots[i],
                                                       [&l_worker](TagArray& raw_tags) -> bool {return l_worker.QueueTags(raw_tags);})
                                                       );

        sleep(1);
    }
    // Start tag detection threads
    for (TagDetectorCamWorker& w: cam_workers){
        w.Start();
        sleep(1);
    }
    // Start localization thread
    l_worker.Start();

    // Wait until the tag detection threads are finished
    for (TagDetectorCamWorker& w: cam_workers){
        w.join();
    }
    std::cout << "All camera workers finished" << std::endl;

    // Wait until localization is finished (never)
    l_worker.join();

    getopt_destroy(getopt);

    return 0;
}
