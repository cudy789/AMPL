#pragma once

#include <utility>
#include <vector>

#include "Pose.h"

class ILocalizationStrategy{
public:

    virtual bool Compute(TagArray& fresh_poses, RobotPose& filtered_pose) = 0;

};

class MeanLocalizationStrategy: public ILocalizationStrategy{
public:
    bool Compute(TagArray& fresh_poses, RobotPose& filtered_pose) override;
};

class LocalizationFilter{
public:
    explicit LocalizationFilter(ILocalizationStrategy* strategy): _strategy(strategy){};

    ~LocalizationFilter(){delete _strategy;}

protected:
    ILocalizationStrategy* _strategy;
    RobotPose _filtered_pose;

};




