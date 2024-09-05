#pragma once

#include <chrono>
#include <iostream>

#include "TimeUtils.h"


inline int STALE_TAG_MS = 500; // If a tag hasn't been seen in this amount of time, then assume we no longer see the tag and clear values
inline size_t NUM_TAG_IDS = 25;


struct Pose {
public:
    double x;
    double y;
    double z;
    double roll;
    double pitch;
    double yaw;

    ulong t = CurrentTime();


    template<typename T>
    Pose operator/=(T val) {
        this->x /= val;
        this->y /= val;
        this->z /= val;
        this->roll /= val;
        this->pitch /= val;
        this->yaw /= val;
        return *this;
    }

    Pose &operator+=(const Pose &o) {
        this->x += o.x;
        this->y += o.y;
        this->z += o.z;
        this->roll += o.roll;
        this->pitch += o.pitch;
        this->yaw += o.yaw;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pose& p) {
        os << "X: " << p.x << ", Y: " << p.y << ", Z: " << p.z
        << ", roll: " << p.roll << ", pitch: " << p.pitch << ", yaw: " << p.yaw;

        return os;
    }

};

struct TagPose : public Pose {
    int tag_id;
    int cam_id;

    // The center of the detection in image pixel coordinates.
    double c[2];

    // The corners of the tag in image pixel coordinates. These always
    // wrap counter-clock wise around the tag.
    double p[4][2];

    friend std::ostream& operator<<(std::ostream& os, const TagPose& o_p) {
        os << "tag_id: " << o_p.tag_id << "," << (Pose)o_p;

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
                if (CurrentTime() -  (*it).t > STALE_TAG_MS * 1.0e6){
                    it = v.erase(it);
                } else{
                    it++;
                    stale_tags++;
                }
            }
        }

        return stale_tags;
    }

    std::vector<std::vector<TagPose>> data;
    size_t _num_tags;
};

