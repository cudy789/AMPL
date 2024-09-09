#pragma once

#include <yaml-cpp/yaml.h>
#include <Eigen>
#include "Logger.h"


struct CamParams{
    std::string name;
    int camera_id;
    Eigen::Matrix3d R_camera_robot {{1, 0, 0},
                                    {0, 0, 1},
                                    {0, -1, 0}}; // 90* rotation, camera is facing same direction as robot
    Eigen::Vector3d T_camera_robot {0,0,0}; // No displacement relative to robot front

    struct {
        float quad_decimate = 2.0; // Decimate input image by this factor
        float quad_sigma = 0.0; // Apply low-pass blur to input
        int nthreads = 1; // Use this many CPU threads for AT detection
        bool debug = false; // Enable debugging output (slow)
        bool refine_edges = true; // Spend more time trying to align edges of tags
    } tag_detector;

    friend std::ostream& operator <<(std::ostream& os, const CamParams& c_params) {
        os << "{camera_id: " << c_params.camera_id << ", R_camera_robot: " << c_params.R_camera_robot
           << ", T_camera_robot: " << c_params.T_camera_robot << ", quad_decimate: " << c_params.tag_detector.quad_decimate
           << ", quad_sigma: " << c_params.tag_detector.quad_sigma << ", nthreads: " << c_params.tag_detector.nthreads
           << ", debug: " << c_params.tag_detector.debug << ", refine_edges: " << c_params.tag_detector.refine_edges
           << "}";

        return os;
    }


};

class ParamParser{
public:
    ParamParser() = delete;

    static std::vector<CamParams> ParseConfig(std::string cfg_file){
        std::vector<CamParams> cam_p;

        Eigen::Matrix3d base_rot_mat;
        base_rot_mat << 1, 0, 0,
                0, 0, 1,
                0, -1, 0;

        Eigen::Vector3d base_disp_mat{0, 0, 0};

        try {
            YAML::Node parser = YAML::LoadFile(cfg_file);
            if (parser["Cameras"]) {
                YAML::Node cameras_yml = parser["Cameras"];
                for (YAML::const_iterator it = cameras_yml.begin(); it != cameras_yml.end(); ++it) {


                    std::string c_name = it->first.as<std::string>();
                    int c_id = it->second["camera_id"].as<int>();

                    cam_p.emplace_back(CamParams{.name = c_name, .camera_id=c_id});

                }
            }
        } catch (const std::exception& e) {
            AppLogger::Logger::Log("Error parsing YAML file: " + std::string(e.what()), AppLogger::SEVERITY::ERROR);
            throw(e);
        }
        return cam_p;
    }



private:
    YAML::Node _yaml_parser;



};