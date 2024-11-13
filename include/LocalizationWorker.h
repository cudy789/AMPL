#pragma once

#include <semaphore>
#include <vector>

#include "Pose.h"
#include "Localization.h"
#include "Worker.h"

/***
 * @brief The threaded Localization worker periodically calculates the robot pose using the default localization
 * strategy on the currently available data.
 *
 * The camera workers register the QueueTags function in their callback and populate this object with tag data each
 * time they receive an image frame. The Execute function in the LocalizationWorker periodically checks the freshness
 * of all tag data and removes any stale tag poses (>150ms old), then applies the localization strategy to calculate
 * the robot pose.
 */
class LocalizationWorker: public Localization, public Worker{

public:
    /***
     * @brief Initialize the worker, set the strategy to be MeanLocalizationStrategy.
     */
    LocalizationWorker();

    /***
     * @brief Add multiple tag poses to be used in the localization calculation.
     * @param raw_tagarray The tags to be added
     * @return true if all tags were successfully added, false otherwise.
     */
    bool QueueTags(TagArray& raw_tagarray);

    /***
     * @brief Add a single tag pose to be used in the localization calculation.
     * @param raw_pose The single tag to be added
     * @return true if the tag was successfully added, false otherwise.
     */
    bool QueueTag(Pose raw_pose);

    /***
     * @brief Get the current calculation of the robot's pose.
     * @return The fresh calculated robot pose in the global frame.
     */
    RobotPose GetRobotPose();

    /***
     * @brief Enable stats logging every 500ms off by default.
     * @param log_stats true to enable stats logging, false to disable.
     */
    void LogStats(bool log_stats);

    private:

    /***
     * @brief Get new tag poses from all registered callback functions, clear stale tag poses (>150ms) , log when a
     * unique tag has been detected or lost, then calculate the robot pose using the chosen strategy. If _log_stats is
     * enabled, periodically log the robot pose.
     */
    void Execute() override;

    std::binary_semaphore _raw_tag_sem{1};
    TagArray _raw_tag_poses;
    TagArray _fresh_tag_poses;
    std::vector<int> _fresh_unique_tags = std::vector<int> (static_cast<int>(NUM_TAG_IDS), 0);
    std::vector<int> _last_unique_tags = std::vector<int> (static_cast<int>(NUM_TAG_IDS), 0);

    std::binary_semaphore _robot_pose_sem{1};
    Pose _robot_pose;

    bool _log_stats = false;
    ulong _last_log_time_ns = CurrentTime();
    ulong _log_period_ns = 5e8; // 500 ms logging rate

};
