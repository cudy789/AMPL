#pragma once

#include <map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <Eigen>


#include "Pose.h"
#include "Logger.h"

using json = nlohmann::json;

inline std::map<int, Pose> TagLayout;


class TagLayoutParser{
public:
    TagLayoutParser() = delete;

    static void ParseConfig(std::string cfg_file){

        try {
            std::ifstream f(cfg_file);

            json json_tags = json::parse(f);

            for (const auto& item: json_tags["fiducials"]){
                std::string family = item["family"];
                int id = item["id"].get<int>();
                double size = item["size"].get<double>();
                int unique = item["unique"].get<int>();
                std::vector<double> transform = item["transform"].get<std::vector<double>>();

                Eigen::Vector3d T_AG{transform[3], transform[7], transform[11]};
                Eigen::Matrix3d R_AG{{transform[0], transform[1], transform[2]},
                                     {transform[4], transform[5], transform[6]},
                                     {transform[8], transform[9], transform[10]}};

                TagLayout.insert({id, Pose{T_AG, R_AG}});

                AppLogger::Logger::Log("Adding tag " + to_string(Pose{T_AG, R_AG}) + " from .fmap file", AppLogger::SEVERITY::INFO);
            }


        } catch (const std::exception& e) {
            AppLogger::Logger::Log("Error parsing JSON file: " + std::string(e.what()), AppLogger::SEVERITY::ERROR);
            throw(e);
        }
    }


};

