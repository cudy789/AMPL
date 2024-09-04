#pragma once

#include <vector>

#include "Pose.h"

class Localization{
public:
    Localization() = default;


    std::vector<std::vector<double>> Stats(std::vector<TagPose>& tag_poses);


    /***
     * Given a vector of fresh tag poses (with multiple Poses per tag), compute a single Pose for each tag
     *
     * @param[in] fresh_tag_poses
     * @return A vector of tag poses that are our best guess for where the tag actually is in the robot frame
     */
    std::vector<TagPose> DisambiguateTags(TagArray& fresh_tag_poses);
};




