#include <gtest/gtest.h>
#include <iostream>

#include "common.h"
#include "TDCamWorker.h"
#include "ConfigParser.h"
#include "MatrixHelpers.h"

TEST(TDCamWorker, HelloWorld){
    SetupLogger("TDCamWorker_HelloWorld");


    Eigen::Matrix3d rotmat;
        rotmat << 1, 0, 0,
        0, 0, 1,
        0, -1, 0;

    Eigen::Vector3d dispmat{0, 0, 0};

    CamParams c1{.name = "C0", .camera_id = 2, .R_camera_robot = rotmat, .T_camera_robot = dispmat};
    std::map<int, Pose_single> tag_layout;

    TDCamWorker w(c1, tag_layout, [this](TagArray& raw_tags) -> bool {return true;}, false);

    w.Start();

    sleep(5);

    // Test stopping & starting
    ASSERT_FALSE(w.Stop(false));

    sleep(5);

    ASSERT_TRUE(w.Stop());
    AppLogger::Logger::Flush();
}

TEST(TDCamWorker, OpenFromYaml){
    SetupLogger("TDCamWorker_OpenFromYaml");

    AMPLParams params = ConfigParser::ParseConfig("../test/test_config.yml");
    std::vector<CamParams>& c_params = params.cam_params;
    std::map<int, Pose_single> tag_layout = TagLayoutParser::ParseConfig("../test/test_tag_layout.fmap");

    AppLogger::Logger::Log("Parsed params from yaml");

    CamParams c1 = c_params[0];

    AppLogger::Logger::Log("Camera params: " + to_string(c1));

    TDCamWorker w(c1, tag_layout, [this](TagArray& raw_tags) -> bool {return true;}, false);

    w.Start();

    sleep(5);

    ASSERT_TRUE(w.Stop());
    AppLogger::Logger::Flush();
}
