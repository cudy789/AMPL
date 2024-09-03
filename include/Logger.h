#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <utility>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace AppLogger {

    enum SEVERITY {
        DEBUG, INFO, WARNING, ERROR, NONE
    };

    inline std::map<SEVERITY, std::string> severityToString{
            {DEBUG,    "DEBUG"},
            {INFO,     "INFO"},
            {WARNING,  "WARNING"},
            {ERROR,    "ERROR"},
    };

    inline std::map<SEVERITY, std::string> severityToColorString{
            {DEBUG,    "\033[1;34mDEBUG\033[0m"}, // blue
            {INFO,     "\033[1;32mINFO\033[0m"}, // green
            {WARNING,  "\033[1;33mWARNING\033[0m"}, // yellow
            {ERROR,    "\033[1;31mERROR\033[0m"}, // red
    };

    using clock = std::chrono::steady_clock;
    using l_ns = std::chrono::duration<ulong, std::nano>;

    inline ulong CurrentTime() {
        std::chrono::time_point<clock, l_ns> t = clock::now();
        return t.time_since_epoch().count();

    }


/***
 * A thread-safe logger for stdout and file logging.
 */
    class Logger {
    public:
        Logger (Logger const&) = delete;
        void operator=(Logger const&) = delete;

        static bool Log(std::string value, SEVERITY level = SEVERITY::INFO) {
            return GetInstance().log(std::move(value), level);
        }

        static void SetVerbosity(SEVERITY verbosity){
            GetInstance().setVerbosity(verbosity);
        }

        static void SetStdout(bool enabled){
            GetInstance().setStdOut(enabled);
        }
        static void SetFileout(bool enabled){
            GetInstance().setFileout(enabled);
        }
        static std::string GetFilePath(){
            return GetInstance().getFilepath();
        }

        static Logger& GetInstance() {
            static Logger instance; // instantiated on first call, guaranteed to be destroyed
            return instance;
        };

        static bool Close(bool terminate=false) {
            return GetInstance().stop(terminate);
        }

    private:
        Logger() {
            _worker_t = new std::thread([this]() { this->Run(); });
            _filepath = "./multicam_apriltag_localization_log.txt";
            _verbosity = INFO;
        };
        ~Logger() {
            if (!_stop){
                Close();
            }
            delete _worker_t;
        }

        bool log(std::string value, SEVERITY level = SEVERITY::INFO) {
            if (_stop) return false;
            if (level >= _verbosity){
                if (_ostream_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(200))) {
                    _ostream_queue.push({value, level});
                    _ostream_sem.release();
                    return true;
                } else{
                    return false;
                }
            } else{
                return false;
            }
        };

        void setStdOut(bool enabled){
            _stdout_enabled = enabled;
        }
        void setFileout(bool enabled){
            _fileout_enabled = enabled;
        }
        void setVerbosity(SEVERITY verbosity){
            _verbosity = verbosity;
        }
        std::string getFilepath(){
            return _filepath;
        }


        std::string datetime() {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%X");
            return ss.str();

        }

        bool stop(bool terminate=false) {
            if (_stop_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(2000))) {
                _stop = true;
                _terminate=terminate;
                _stop_sem.release();
                _worker_t->join();
                return true;
            } else {
                _stop_sem.release();
                return false;
            }
        }

        void Run() {
            ulong start_ns = CurrentTime();
            bool stop = false;

            while (!stop) {
                // Check to see if we should stop the worker
                if (_stop_sem.try_acquire()) {
                    if (_terminate){
                        _stop_sem.release();
                        break;
                    }
                    if (_stop) {
                        stop = true;
                    }
                    _stop_sem.release();
                }

                // Check the queue for new messages to log to stdout
                while (!_ostream_queue.empty()) {
                    std::pair<std::string, SEVERITY> item = _ostream_queue.front();
                    _ostream_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000));
                    _ostream_queue.pop();
                    _ostream_sem.release();
                    if (_stdout_enabled){
                        std::stringstream ss;
                        ss << "[" << datetime() << "] " << severityToColorString[item.second] << ": " << item.first << std::endl;
                        std::cout << ss.str();
                    }
                    if (_fileout_enabled){
                        _filestream_queue.push(item);
                    }

                }


                // Check to see if enough time has passed and we should log messages to the logfile, or the thread is stopping
                // Don't perform this check if fileout is disabled.
                if ((CurrentTime() - start_ns > _thread_write_period_ns || stop) && _fileout_enabled) {
                    start_ns = CurrentTime();

                    std::ofstream *log_file;

                    log_file = new std::ofstream(_filepath, std::ios::app); // try to open and append to file
                    if (!(*log_file)) { // if file DNE, create new file
                        log_file = new std::ofstream(_filepath);
                    }

                    while (!_filestream_queue.empty()) {
                        std::pair<std::string, SEVERITY> item = _filestream_queue.front();
                        _filestream_queue.pop();

                        std::stringstream ss;
                        ss << "[" << datetime() << "] " << severityToString[item.second] << ": " << item.first << std::endl;
                        (*log_file) << ss.str();
                    }
                    log_file->close();
                    delete log_file;
                }

            }

        }



        bool _stop = false;
        bool _terminate = false;
        std::binary_semaphore _stop_sem{1};

        std::thread *_worker_t;
        std::binary_semaphore _ostream_sem{1};
        std::queue<std::pair<std::string, SEVERITY>> _ostream_queue;

        std::queue<std::pair<std::string, SEVERITY>> _filestream_queue;
        ulong _thread_write_period_ns = 5 * 1.0e6; // write to file every 5 seconds
        std::string _filepath;

        SEVERITY _verbosity;
        bool _stdout_enabled = true;
        bool _fileout_enabled = true;

    };

}