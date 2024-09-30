#pragma once

#include <utility>
#include <vector>

#include "Pose.h"
#include "../lib/dkm/dkm.hpp"

class ILocalizationStrategy{
public:

    virtual bool Compute(TagArray& fresh_poses, RobotPose& filtered_pose) = 0;

};

class MeanLocalizationStrategy: public ILocalizationStrategy{
public:
    bool Compute(TagArray& fresh_poses, RobotPose& filtered_pose) override;
};

/*
 * Choose the pose that the most tags agree with. Consider both possible pose locations for every visible tag. Only use
 * unique tags per camera (i.e. only the newest tag if there are multiple detections of the same tag on the same camera)
 *
 * NEW
 * Cluster similar poses. Select the geometric mean of the cluster with the
 *      1. Most amount of points (?)
 *      2. Lowest error (?)
 *      2. Substantial # of points and lowest error (?)
 *
 */
class KMeansLocalizationStrategy: public ILocalizationStrategy{
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




