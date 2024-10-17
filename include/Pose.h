#pragma once

#include <chrono>
#include <iostream>
#include <Eigen>

#include "TimeUtils.h"
#include "MatrixHelpers.h"
#include "Logger.h"


inline int STALE_TAG_MS = 150; // If a tag hasn't been seen in this amount of time, then assume we no longer see the tag and clear values
inline size_t NUM_TAG_IDS = 25;

struct Pose_single {
    // Translation (x,y,z) and rotation (rotation matrix RPY)
    Eigen::Vector3d T = Eigen::Vector3d::Constant(0);
    Eigen::Matrix3d R = Eigen::Matrix3d::Constant(0);

    Pose_single operator-(const Pose_single &o){
        Pose_single diff;

        diff.T = T - o.T;
        diff.R = R - o.R;

        return diff;
    }
    Pose_single operator+(const Pose_single &o){
        Pose_single sum;

        sum.T = T + o.T;
        sum.R = R + o.R;

        return sum;
    }
    template<typename T>
    Pose_single operator*(T val) {
        Pose_single prod;

        prod.T = this->T * val;
        prod.R = this->R * val;

        return prod;
    }

    template<typename T>
    Pose_single operator/=(T val) {
        this->T /= val;
        this->R /= val;
        return *this;
    }

    Pose_single &operator+=(const Pose_single &o) {
        this->T += o.T;
        this->R += o.R;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pose_single& p) {
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
    // The time this pose object was created at
    ulong time = CurrentTime();

    // The center of the detection in image pixel coordinates.
    double c[2];

    // The corners of the tag in image pixel coordinates. These always
    // wrap counter-clock wise around the tag.
    double p[4][2];

    // The pose of the object relative to the AprilTag, camera, robot, and the global field coordinates
    Pose_single tag;
    Pose_single camera;
    Pose_single robot;
    Pose_single global;

    // The error of the tag detection
    double err;

    friend std::ostream& operator<<(std::ostream& os, const Pose& o_p) {
        os << "tag_id: " << o_p.tag_id << ", " << "cam_id: " << o_p.cam_id << ", err: " << o_p.err
           << ", GLOBAL_FRAME: " << o_p.global << ", ROBOT_FRAME: " << o_p.robot << ", CAM_FRAME: " << o_p.camera;
        return os;
    }

};

struct RobotPose: public Pose {
    // No camera or tag_id associated with this class
//    int tag_id = -1;
//    int cam_id = -1;

    friend std::ostream& operator<<(std::ostream& os, const RobotPose& o_p) {
        os << o_p.global;
        return os;
    }

};

struct TagArray{
    explicit TagArray(){
        _num_tags = 0;
        data = std::vector<std::vector<Pose>>{NUM_TAG_IDS};
    }

    void ClearAll(){
        _num_tags = 0;
        data = std::vector<std::vector<Pose>>{NUM_TAG_IDS};
    }
    int ClearStale(){
        int stale_tags = 0;
        for (std::vector<Pose>& v: data){
            for (auto it=v.begin(); it!=v.end();){
                if ((CurrentTime() -  (*it).time) > STALE_TAG_MS * 1.0e6){
                    it = v.erase(it);
                    stale_tags++;
                } else{
                    it++;
                }
            }
        }
        if (stale_tags > _num_tags){
            AppLogger::Logger::Log("ERROR, more stale tags than total number of tags. stale_tags=" + std::to_string(stale_tags) + ", _num_tags=" + std::to_string(_num_tags), AppLogger::SEVERITY::ERROR);
            exit(1);

        }
        _num_tags -= stale_tags;
        return stale_tags;
    }

    std::vector<std::vector<Pose>> data;
    size_t _num_tags = 0;
};

