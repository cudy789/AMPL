#pragma once

#include <yaml-cpp/yaml.h>
#include <Eigen>
#include "Logger.h"


struct CamParams{
    std::string name;
    int camera_id;

    // Resolution x, y
    int rx;
    int ry;

    // Focal length (in pixels), x and y
    // focalLengthPxX = focalLengthPxY = px/mm * focalLengthMm
    int fx;
    int fy;

    // Desired FPS
    float fps;

    // Exposure, range varies per camera
    int exposure;

    Eigen::Matrix3d R_camera_robot {{1, 0, 0},
                                    {0, 0, 1},
                                    {0, -1, 0}}; // 90* rotation, camera is facing same direction as robot
    Eigen::Vector3d T_camera_robot {0,0,0}; // No displacement relative to robot geometric center

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

// Assuming the origin is at the 3D geometric center of the robot, use the following frame axis convention:
//   x: positive right
//   y: positive forwards
//   z: positive up
// Define your camera locations using the above axis. For example, a camera on the front of the robot (50cm from the center
// of the robot), centered left-right, mounted on a pole 25cm above the midpoint of the robot looking upwards
// at a 20 degree angle:
// translation: [0.0, 0.5, 0.25] # x,y,z
// rotation: [0, 20, 0] # roll, pitch, yaw
// For the same configuration, but at the back of the robot looking backwards:
// translation: [0, -0.5, 0.25] # x,y,z
// rotation: [0, 20, 180] # roll, pitch, yaw
class ParamParser{
public:
    ParamParser() = delete;

    static std::vector<CamParams> ParseConfig(std::string cfg_file){
        std::vector<CamParams> cam_p;

        Eigen::Matrix3d R_base {{1, 0, 0},
                               {0, 0, 1},
                               {0, -1, 0}}; // 90* rotation, camera is facing same direction as robot


        try {
            YAML::Node parser = YAML::LoadFile(cfg_file);
            if (parser["Cameras"]) {
                YAML::Node cameras_yml = parser["Cameras"];
                for (YAML::const_iterator it = cameras_yml.begin(); it != cameras_yml.end(); ++it) {


                    std::string c_name = it->first.as<std::string>();
                    int c_id = it->second["camera_id"].as<int>();
                    int c_rx = it->second["rx"].as<int>();
                    int c_ry = it->second["ry"].as<int>();
                    int c_fx = it->second["fx"].as<int>();
                    int c_fy = it->second["fy"].as<int>();
                    float c_fps = it->second["fps"].as<float>();
                    int c_exposure = it->second["exposure"].as<int>();

                    Eigen::Vector3d T_total = Eigen::Vector3d(it->second["translation"].as<std::vector<double>>().data());
                    Eigen::Vector3d c_rotation = Eigen::Vector3d(it->second["rotation"].as<std::vector<double>>().data());

                    Eigen::Matrix3d R_total = CreateRotationMatrix((Eigen::Vector3d)c_rotation) * R_base; // add the two rotations together by matrix multiplication, R2 * R1

                    cam_p.emplace_back(CamParams{.name=c_name, .camera_id=c_id, .rx=c_rx, .ry=c_ry,
                                                 .fx=c_fx, .fy=c_fy, .fps=c_fps, .exposure=c_exposure,
                                                 .R_camera_robot=R_total, .T_camera_robot=T_total});

                    AppLogger::Logger::Log("created rotation matrix " + to_string(R_total));

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