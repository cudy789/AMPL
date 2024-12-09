#pragma once
#include "Logger.h"
#include "Pose.h"

namespace AppLogger{

    /**
     * @brief A singleton threaded csv trajectory logger class. Intended to log robot trajectories to a csv file with timestamps.
     * Non-blocking and high performant.
     */
    class TrajectoryLogger: public Logger{
    public:
        /**
        * @brief No copy constructor allowed.
        */
        TrajectoryLogger(const TrajectoryLogger&) = delete;
        /**
         * @brief No assignment operator allowed.
         */
        void operator=(const TrajectoryLogger&) = delete;
        /**
         * @brief Get the singleton instance of TrajectoryLogger.
         * @return The singleton instance.
         */
        static TrajectoryLogger& GetInstance() {
            static TrajectoryLogger instance; // Independent static instance for TrajectoryLogger
            return instance;
        }
        /**
         * @brief Log global RobotPose values in .csv format. Split into global x,y,z,r,p,y
         * @param value
         * @param level
         * @return
         */
        static bool Log(const RobotPose& value, SEVERITY level = SEVERITY::INFO) {
            Eigen::Vector3d rpy = RotationMatrixToRPY(value.global.R);

            std::string pose_json = to_string(CurrentTime()) + ", " + to_string(value.global.T[0]) + ", "
                    + to_string(value.global.T[1]) + ", " + to_string(value.global.T[2])
                    + ", " + to_string(rpy[0]) + ", " + to_string(rpy[1]) + ", " + to_string(rpy[2]);

            return TrajectoryLogger::GetInstance().log(pose_json, level);
        }
        /**
         * @brief Set the minimum verbosity of logging messages to be recorded. Defaults to INFO.
         * @param verbosity The verbosity.
         */
        static void SetVerbosity(SEVERITY verbosity){
            GetInstance().setVerbosity(verbosity);
        }
        /**
         * @brief Enable or disable logging to stdout.
         * @param enabled Enable stdout logging if true, disable if false.
         */
        static void SetStdout(bool enabled){
            GetInstance().setStdOut(enabled);
        }
        /**
         * @brief Check if logging to stdout.
         * @return true if logging to stdout, false otherwise.
         */
        static bool GetStdout(){
            return GetInstance().getStdOut();
        }
        /**
         * @brief Enable or disable logging to the logfile.
         * @param enabled Enable logfile logging if true, disable if false.
         */
        static void SetFileout(bool enabled){
            GetInstance().setFileout(enabled);
        }
        /**
         * @brief Check if logfile output is enabled
         * @return logfile logging if true, no logfile logging if false.
         */
        static bool GetFileout(){
            return GetInstance().getFileout();
        }
        /**
         * @brief Get the logging file path.
         * @return The path to the logging file relative to the directory the main executable was started from.
         */
        static std::string GetFilePath(){
            return GetInstance().getFilepath();
        }
        /**
         * @brief Set the logging file path.
         * @param filepath The path to the logging file relative to the directory the main executable was started from.
         */
        static void SetFilepath(const std::string& filepath){
            GetInstance().setFilepath(filepath);
        }
        /**
         * @brief Flush stdout and fstream. Block until the operation completes.
         * @return true if all data was successfully flushed, false if not.
         */
        static bool Flush(){
            return GetInstance().flush();
        }
        /**
         * @brief Stop the logger.
         * @param terminate If terminate=true forcefully stop the logger without flushing messages, otherwise gracefully
         * stop the logger and allow the messages to be flushed.
         * @return true if all data was successfully flushed, false otherwise.
         */
        static bool Close(bool terminate=false) {
            return GetInstance().stop(terminate);
        }

    protected:

        TrajectoryLogger(): Logger() {
            _filepath = "./logs/ampl_trajectory_log_" + datetime_ms() + ".csv";
            _stdout_enabled = false;
            log("time_ms, x, y, z, roll, pitch, yaw");
        }
        ~TrajectoryLogger(){
            stop();
        }


        /**
         * @brief Put timestamp first, then .csv formatted RobotPose data second.
         * @param data_tuple
         * @return
         */
        std::stringstream messageFormatHelper(const std::tuple<std::string, SEVERITY>& data_tuple) override {
            std::stringstream ss;
            ss << std::get<0>(data_tuple) << std::endl;
            return ss;
        }



    };

}