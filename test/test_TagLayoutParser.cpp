#include <gtest/gtest.h>
#include <iostream>

#include "common.h"
#include "TagLayoutParser.h"

TEST(TagLayoutParser, TestLayoutJson){
    SetupLogger("TagLayoutParser_TestLayoutJson");


    TagLayoutParser::ParseConfig("../test/test_tag_layout.fmap");



    AppLogger::Logger::Flush();


}