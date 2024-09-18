#pragma once

#include <chrono>
#include <iostream>
#include <Eigen>

#include "TimeUtils.h"
#include "MatrixHelpers.h"


inline int STALE_TAG_MS = 150; // If a tag hasn't been seen in this amount of time, then assume we no longer see the tag and clear values
inline size_t NUM_TAG_IDS = 25;

// Global frame = G
// AprilTag Frame = A
// Robot Frame = R
// Camera Frame = C

// TODO refactor tagpose to contain two pose objects, one for robot frame, the other for global frame.

struct Pose {
    // Translation (x,y,z) and rotation (rotation matrix RPY)
    Eigen::Vector3d T;
    Eigen::Matrix3d R;

    ulong time = CurrentTime();


    template<typename T>
    Pose operator/=(T val) {
        this->T /= val;
        this->R /= val;
        return *this;
    }

    Pose &operator+=(const Pose &o) {
        this->T += o.T;
        this->R += o.R;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pose& p) {
        os << "XYZ: " << p.T
           << "RPY: " << RotationMatrixToRPY(p.R);

        return os;
    }

};

struct TagPose : public Pose {
    int tag_id;
    int cam_id;

    // AprilTag's position x,y,z and rotation in the global (field) frame of reference. This is constant, as the tag never moves. These values come from the field configuration file
    Eigen::Vector3d T_G;
    Eigen::Matrix3d R_G;

    // The error of the tag
    double err;

    // The center of the detection in image pixel coordinates.
    double c[2];

    // The corners of the tag in image pixel coordinates. These always
    // wrap counter-clock wise around the tag.
    double p[4][2];

    friend std::ostream& operator<<(std::ostream& os, const TagPose& o_p) {
        os << "tag_id: " << o_p.tag_id << ", " << "cam_id: " << o_p.cam_id << ", err: " << o_p.err
           << ", GLOBAL XYZ: " << o_p.T_G << ", GLOBAL RPY: " << RotationMatrixToRPY(o_p.R_G) << ", "
           << (Pose)o_p;

        return os;
    }
};


struct TagArray{
    explicit TagArray(){
        _num_tags = NUM_TAG_IDS;
        data = std::vector<std::vector<TagPose>>{_num_tags};
    }

    void ClearAll(){
        data = std::vector<std::vector<TagPose>>{_num_tags};
    }
    int ClearStale(){
        int stale_tags = 0;
        for (std::vector<TagPose>& v: data){
            for (auto it=v.begin(); it!=v.end();){
                if ((CurrentTime() -  (*it).time) > STALE_TAG_MS * 1.0e6){
                    it = v.erase(it);
                    stale_tags++;
                } else{
                    it++;
                }
            }
        }

        return stale_tags;
    }

    std::vector<std::vector<TagPose>> data;
    size_t _num_tags;
};

