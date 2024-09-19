#pragma once

#include <chrono>
#include <iostream>
#include <Eigen>

#include "TimeUtils.h"
#include "MatrixHelpers.h"


inline int STALE_TAG_MS = 150; // If a tag hasn't been seen in this amount of time, then assume we no longer see the tag and clear values
inline size_t NUM_TAG_IDS = 25;

struct Pose_base {
    // Translation (x,y,z) and rotation (rotation matrix RPY)
    Eigen::Vector3d T = Eigen::Vector3d::Constant(0);
    Eigen::Matrix3d R = Eigen::Matrix3d::Constant(0);

    Pose_base operator-(const Pose_base &o){
        Pose_base diff;

        diff.T = T - o.T;
        diff.R = R - o.R;

        return diff;
    }
    Pose_base operator+(const Pose_base &o){
        Pose_base sum;

        sum.T = T + o.T;
        sum.R = R + o.R;

        return sum;
    }
    template<typename T>
    Pose_base operator*(T val) {
        Pose_base prod;

        prod.T = this->T * val;
        prod.R = this->R * val;

        return prod;
    }

    template<typename T>
    Pose_base operator/=(T val) {
        this->T /= val;
        this->R /= val;
        return *this;
    }

    Pose_base &operator+=(const Pose_base &o) {
        this->T += o.T;
        this->R += o.R;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pose_base& p) {
        os << "XYZ: " << p.T
           << " RPY: " << RotationMatrixToRPY(p.R);

        return os;
    }
};

struct Pose{
    // The tag this pose originated from
    int tag_id = -1;
    // The camera that detected this tag
    int cam_id = -1;

    // The pose of the object relative to the AprilTag, camera, robot, and the global field coordinates
    Pose_base tag;
    Pose_base camera;
    Pose_base robot;
    Pose_base global;

    // The error of the tag detection
    double err;

    friend std::ostream& operator<<(std::ostream& os, const Pose& o_p) {
        os << "tag_id: " << o_p.tag_id << ", " << "cam_id: " << o_p.cam_id << ", err: " << o_p.err
           << ", GLOBAL_FRAME: " << o_p.global << ", ROBOT_FRAME: " << o_p.robot << ", TAG_FRAME: " << o_p.tag;
        return os;
    }

};

struct Pose_t: public Pose {

    ulong time = CurrentTime();

};

struct PoseCv: public Pose_t {

    // The center of the detection in image pixel coordinates.
    double c[2];

    // The corners of the tag in image pixel coordinates. These always
    // wrap counter-clock wise around the tag.
    double p[4][2];


};

struct RobotPose: public Pose {
    // No camera or tag_id associated with this class
//    int tag_id = -1;
//    int cam_id = -1;

};

//struct FilterPose {
//    Pose_t pose_global_frame;
//};




struct TagArray{
    explicit TagArray(){
        _num_tags = NUM_TAG_IDS;
        data = std::vector<std::vector<PoseCv>>{_num_tags};
    }

    void ClearAll(){
        data = std::vector<std::vector<PoseCv>>{_num_tags};
    }
    int ClearStale(){
        int stale_tags = 0;
        for (std::vector<PoseCv>& v: data){
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

    std::vector<std::vector<PoseCv>> data;
    size_t _num_tags;
};

