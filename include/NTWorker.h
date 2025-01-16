#pragma once

#include "Worker.h"
#include "Pose.h"

#include <networktables/DoubleArrayTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

/**
 * @brief The threaded NetworkTables worker periodically retrieves the latest robot pose from the LocalizationWorker
 * and publishes the global frame pose data to NetworkTables. By default, the worker will attempt to connect to the
 * NetworkTables server at 10.TE.AM.2, where TEAM is the FRC team number. You may also specify a hostname to
 * connect to instead, i.e. 127.0.0.1, 192.168.1.213, etc..
 *
 * Other clients connected to the same NetworkTables server will see data being published at:
 * * MAPLE/location: double[pos_x, pos_y, pos_z] in meters
 * * MAPLE/orientation: double[roll, pitch, yaw] in degrees
 *
 * Compatible with NetworkTables 4.
 */
class NTWorker: public Worker{

public:
    /**
     * @brief Create the worker, default NetworkTables client connection to 127.0.0.1. Setup the publishers to position
     * and orientation.
     */
    NTWorker();
    /**
     * @brief Create the worker, connect to NetworkTables via FRC team number.
     * @param team_num The FRC team number.
     */
    NTWorker(int team_num);
    /**
     * @brief Create the worker, connect to NetworkTables via hostname string.
     * @param hostname The hostname with the NetworkTables server running.
     */
    NTWorker(std::string hostname);

    /**
     * @brief Register the pose callback function that is periodically called to retrieve the latest pose data to publish
     * to NetworkTables.
     * @param pose_callback The callback function which returns a RobotPose object to be parsed and published to NetworkTables.
     */
    void RegisterPoseCallback(const std::function<RobotPose()>& pose_callback);
    /**
     * @brief Check if the worker is connected to the NetworkTables server.
     * @return true if connected to a NetworkTables server, false otherwise.
     */
    bool IsConnected();

private:
    /**
     * @brief Start the NT4 client using either the team number if present, otherwise hostname. If the client fails to
     * connect, wait 10 seconds, then restart the thread.
     */
    void Init() override;
    /**
     * @brief Execute pose callback function to obtain the latest robot pose estimate, parse into the two NetworkTables
     * topics, then publish the values.
     */
    void Execute() override;

    int _team_num = -1;
    std::string _hostname = "127.0.0.1";

    nt::NetworkTableInstance _nt_instance;
    std::shared_ptr<nt::NetworkTable> _nt_table;

    nt::DoubleArrayPublisher _position;
    nt::DoubleArrayPublisher _orientation;

    std::function<RobotPose()> _pose_callback;
};