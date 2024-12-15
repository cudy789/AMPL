#pragma once

#include <utility>
#include <vector>

#include "Pose.h"
#include "../lib/dkm/dkm.hpp"

/**
 * @brief The localization strategy interface class. All children must implement the Compute function with their
 * localization algorithm.
 */
class ILocalizationStrategy{
public:
    /**
     * @brief Calculate the location of the robot in 3d space.
     * @param fresh_poses An array of poses recently received from the cameras. More than one pose per tag per camera
     * is likely to be received, as all possible poses per Apriltag are computed.
     * @param filtered_pose The calculated location of the robot.
     * @return true if at least one tag was used to calculate the filtered_pose of the robot. false if no tags were used to calculate
     * the filtered_pose.
     */
    virtual bool Compute(TagArray& fresh_poses, RobotPose& filtered_pose) = 0;

};

/**
 * @brief For each tag in a given camera, only use the tag pose that has the lowest error. Then, compute the average of
 * each tag pose and use this as the filtered_pose. This method is the default strategy for localization.
 */
class MeanLocalizationStrategy: public ILocalizationStrategy{
public:
    void MaxRateChange(const Pose_single& a, Pose_single& b, double d_rot, double d_linear);

    bool Compute(TagArray& fresh_poses, RobotPose& filtered_pose) override;
};

/**
 * @brief EXPERIMENTAL
 *
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

/**
 * @brief The base localization class which has the localization strategy and the latest filtered pose.
 */
class Localization{
public:
    /**
     * @brief Create the Localization class with the given concrete strategy.
     * @param strategy The concrete strategy class which will be used to compute the robot pose.
     */
    explicit Localization(ILocalizationStrategy* strategy): _strategy(strategy){};

    ~Localization(){delete _strategy;}

protected:
    ILocalizationStrategy* _strategy;
    RobotPose _filtered_pose;

};




