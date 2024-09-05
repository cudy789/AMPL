#include <gtest/gtest.h>
#include <iostream>

#include "common.h"
#include "TDCamWorker.h"

TEST(TDCamWorker, HelloWorld){
    SetupLogger("TDCamWorker_HelloWorld");


    Eigen::Matrix3d rotmat;
        rotmat << 1, 0, 0,
        0, 0, 1,
        0, -1, 0;

    Eigen::Vector3d dispmat{0, 0, 0};

    CamParams c1{.camera_id = 2, .R_camera_robot = rotmat, .T_camera_robot = dispmat};
    TDCamWorker w(c1,[this](TagArray& raw_tags) -> bool {return true;}, false);

    w.Start();

    sleep(15);

    ASSERT_TRUE(w.Stop());
}
