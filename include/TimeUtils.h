#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

/**
 * @brief Get the current system time in milliseconds using std::chrono::high_resolution_clock.
 * @return The system time in milliseconds.
 */
inline ulong CurrentTime() {
    using clock = std::chrono::high_resolution_clock;
    using l_ns = std::chrono::duration<ulong, std::nano>;

    std::chrono::time_point<clock, l_ns> t = clock::now();
    return t.time_since_epoch().count();

}

/**
 * @brief Get the current system time as a datetime formatted YYYY-MM-DD_HH:MM:SS:XXX with millisecond level resolution.
 * @return A string representation of the current datetime.
 */
// https://stackoverflow.com/questions/24686846/get-current-time-in-milliseconds-or-hhmmssmmm-format
inline std::string datetime_ms() {

    auto now = std::chrono::high_resolution_clock::now();
    auto in_time_t = std::chrono::high_resolution_clock::to_time_t(now);

    auto ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%X");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}