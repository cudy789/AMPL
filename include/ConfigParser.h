#pragma once

#include <yaml-cpp/yaml.h>
#include <Eigen>
#include "Logger.h"

/**
 * @brief A struct containing the unique camera parameters for each camera connected to AMPL.
 */
struct CamParams{

    /**
     * @brief Camera name.
     */
    std::string name;
    /**
     * @brief Camera ID, the video number X in /dev/videoX
     */
    int camera_id;
    /**
     * @brief If playing video back from a file, use this filepath.
     */
    std::string camera_playback_file;
    /**
     * @brief Camera resolution x.
     */
    int rx;
    /**
     * @brief Camera resolution y.
     */
    int ry;

    // Focal length (in pixels), x and y
    // focalLengthPxX = focalLengthPxY = px/mm * focalLengthMm
    /**
     * @brief Focal length (in pixels) x. fx = pixels/mm & focalLengthMm
     */
    int fx;
    /**
     * @brief Focal length (in pixels) y. fy = pixels/mm & focalLengthMm
     */
    int fy;

    /**
     * @brief Desired FPS
     */
    float fps;

    /**
     * @brief Exposure. Range varies per camera.
     */
    int exposure;

    Eigen::Matrix3d R_camera_robot {{1, 0, 0},
                                    {0, 1, 0},
                                    {0, 0, 1}}; // Converts from AT frame to robot frame AND includes 90* rotation to align camera Z axis with robot Z axis TODO update note properly

    Eigen::Vector3d T_camera_robot {0,0,0}; // No displacement relative to robot geometric center
    /**
     * @brief Apriltag detector parameters
     */
    struct {
        /**
         * @brief Decimate input image by this factor.
         */
        float quad_decimate = 1.0;
        /**
         * @brief Apply low-pass blur with this sigma value.
         */
        float quad_sigma = 0.0; // Apply low-pass blur to input
        /**
         * Use this many CPU threads for Apriltag detection
         */
        int nthreads = 1; // Use this many CPU threads for AT detection
        /**
         * Enable Apriltag library debugging output (slow)
         */
        bool debug = false;
        /**
         * Spend more time trying to align edges of tags.
         */
        bool refine_edges = true;
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
/**
 * @brief Global AMPL parameters, including the FRC team number and all camera parameter structs.
 */
struct AMPLParams {
    /**
     * @brief The FRC team number. Used to set static IP of the AMPL client and connect to NetworkTables.
     */
    int team_num;

    /**
     * @brief The .fmap file describing the global locations of Apriltags. See TagLayoutParser for more details.
     */
    std::string fmap_file;

    /**
     * @brief Enable/disable RobotPose trajectory logging.
     */
    bool pose_logging;

    /**
     * @brief Enable/disable camera video recording
     */
    bool video_recording;

    /**
     * @brief A vector of camera parameters, one struct per camera.
     */
    std::vector<CamParams> cam_params;

    friend std::ostream& operator <<(std::ostream& os, const AMPLParams& param) {
        os << "team_num: : " << param.team_num << ", camera parameters:";

        for (const auto& c: param.cam_params){
            os << "\t\t" << c;
        }
        return os;
    }
};

/**
 * @brief The .yaml configuration parser. Parses all AMPLParams and CamParams parameters.
 */
class ConfigParser{
public:
    ConfigParser() = delete;

    /**
     * @brief Parse the configuration .yaml file for AMPL parameters, including FRC team number, .fmap file,
     * and camera parameters.
     * @param cfg_file The path to your config.yaml file.
     * @return A struct of configuration parameters populated by the specified .yaml file.
     */
    static AMPLParams ParseConfig(std::string cfg_file){
        AMPLParams params;
        std::vector<CamParams>& cam_p = params.cam_params;

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


                    std::string camera_playback_file;
                    if (it->second["camera_playback_file"]){
                        camera_playback_file = it->second["camera_playback_file"].as<std::string>();
                    }

                    Eigen::Vector3d T_total = Eigen::Vector3d(it->second["translation"].as<std::vector<double>>().data());
                    Eigen::Vector3d c_rotation = Eigen::Vector3d(it->second["rotation"].as<std::vector<double>>().data());

                    Eigen::Matrix3d R_total = CreateRotationMatrix((Eigen::Vector3d)c_rotation);

                    cam_p.emplace_back(CamParams{.name=c_name, .camera_id=c_id, .camera_playback_file=camera_playback_file,
                                                 .rx=c_rx, .ry=c_ry,
                                                 .fx=c_fx, .fy=c_fy, .fps=c_fps, .exposure=c_exposure,
                                                 .R_camera_robot=R_total, .T_camera_robot=T_total});

                    AppLogger::Logger::Log("created rotation matrix " + to_string(R_total));

                }
            }
            if (parser["Team number"]){
                params.team_num = parser["Team number"].as<int>();
                AppLogger::Logger::Log("Team number " + to_string(params.team_num));
            }
            else {
                AppLogger::Logger::Log("No Team number found", AppLogger::SEVERITY::WARNING);
                params.team_num = -1;
            }
            // Expect an fmap file, throw an exception otherwise
            params.fmap_file = parser["Fmap file"].as<std::string>();
            AppLogger::Logger::Log("Fmap file: " + params.fmap_file);

            if (parser["Pose logging"]){
                params.pose_logging = parser["Pose logging"].as<bool>();
            } else{
                params.pose_logging = false;
            }

            if (parser["Video recording"]){
                params.video_recording = parser["Video recording"].as<bool>();
            } else{
                params.video_recording = false;
            }


        } catch (const std::exception& e) {
            AppLogger::Logger::Log("Error parsing YAML file: " + std::string(e.what()), AppLogger::SEVERITY::ERROR);
            throw(e);
        }
        return params;
    }


private:
    YAML::Node _yaml_parser;

};