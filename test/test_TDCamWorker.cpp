#include <gtest/gtest.h>
#include <iostream>

#include "common.h"
#include "TDCamWorker.h"
#include "ParamParser.h"
#include "MatrixHelpers.h"

TEST(TDCamWorker, HelloWorld){
    SetupLogger("TDCamWorker_HelloWorld");


    Eigen::Matrix3d rotmat;
        rotmat << 1, 0, 0,
        0, 0, 1,
        0, -1, 0;

    Eigen::Vector3d dispmat{0, 0, 0};

    CamParams c1{.name = "C0", .camera_id = 2, .R_camera_robot = rotmat, .T_camera_robot = dispmat};

    TDCamWorker w(c1,[this](TagArray& raw_tags) -> bool {return true;}, false);

    w.Start();

    sleep(5);

    ASSERT_TRUE(w.Stop());
    AppLogger::Logger::Flush();
}

TEST(TDCamWorker, OpenFromYaml){
    SetupLogger("TDCamWorker_OpenFromYaml");

    std::vector<CamParams> c_params = ParamParser::ParseConfig("../test/test_config.yml");

    AppLogger::Logger::Log("Parsed params from yaml");

    CamParams c1 = c_params[0];

    AppLogger::Logger::Log("Camera params: " + to_string(c1));

    TDCamWorker w(c1,[this](TagArray& raw_tags) -> bool {return true;}, false);

    w.Start();

    sleep(5);

    ASSERT_TRUE(w.Stop());
    AppLogger::Logger::Flush();
}
