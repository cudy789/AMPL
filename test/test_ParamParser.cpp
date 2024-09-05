#include <gtest/gtest.h>
#include <iostream>

#include "common.h"
#include "TDCamWorker.h"
#include "ParamParser.h"

TEST(ParamParser, TestConfigYaml){
    SetupLogger("ParamParser_TestConfigYaml");

    std::vector<CamParams> c_params = ParamParser::ParseConfig("../test/test_config.yml");

    ASSERT_EQ(c_params.size(), 2);
    for (int i=0; i<c_params.size(); i++){
        AppLogger::Logger::Log("Camera ID: " + std::to_string(c_params[i].camera_id));
        ASSERT_EQ(i, c_params[i].camera_id);
    }
    AppLogger::Logger::Flush();
}