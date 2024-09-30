#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <utility>
#include <thread>

#include "TimeUtils.h"

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


/***
 * A singleton thread safe logger for stdout and file logging.
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

        static void SetFilepath(std::string filepath){
            GetInstance().setFilepath(filepath);
        }

        static Logger& GetInstance() {
            static Logger instance; // instantiated on first call, guaranteed to be destroyed
            return instance;
        }

        static bool Flush(){
            return GetInstance().flush();
        }

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
                std::string date;
                if (_ostream_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(200))) {
                    date = "[" + datetime_ms() + "] ";
                    _ostream_queue.push({value, date, level});
                    _ostream_sem.release();
                    _ostream_data_present_sem.release(); // Notify thread that data is available
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
            if (_filepath_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                std::string filepath = _filepath;
                _filepath_sem.release();
                return filepath;
            }
            return "";
        }

        bool setFilepath(std::string& filepath){
            if (_filepath_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                _filepath = filepath;
                _filepath_sem.release();
                return true;
            }
            return false;
        }

        bool stop(bool terminate=false) {
            if (_stop_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(2000))) {
                _stop = true;
                _terminate=terminate;
                _stop_sem.release();
                _ostream_data_present_sem.release();
                _worker_t->join();
                return true;
            } else {
//                _stop_sem.release();
                _ostream_data_present_sem.release();

                return false;
            }
        }

        bool flush(){
            if (_flush_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(3000))) {
                _flush = true;
                _flush_sem.release();
                _ostream_data_present_sem.release();

                while (true){
                    if (_flush_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                        if (!_flush){
                            _flush_sem.release();
                            break;
                        }
                        _flush_sem.release();
                    }
                    std::this_thread::yield();
                }
                return true;
            } else {
                return false;
            }
        }

        void Run() {
            ulong start_ns = CurrentTime();
            bool stop = false;
            bool flush = false;

            while (!stop) {
                _ostream_data_present_sem.acquire(); // Wait until new data is present

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
                // Check to see if we should flush all data to the fileout
                if (_flush_sem.try_acquire()){
                    flush = _flush;
                    _flush_sem.release();
                }

                // Check the queue for new messages to log to stdout
                while (!_ostream_queue.empty()) {
                    std::tuple<std::string, std::string, SEVERITY> item = _ostream_queue.front();
                    if (_ostream_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                        _ostream_queue.pop();
                        _ostream_sem.release();
                        if (_stdout_enabled){
                            std::stringstream ss;
                            ss << std::get<1>(item) << severityToColorString[std::get<2>(item)] << ": " << std::get<0>(item) << std::endl;
                            std::cout << ss.str();
                        }
                        if (_fileout_enabled){
                            _filestream_queue.push(item);
                        }
                    }

                }

                // Check to see if enough time has passed and we should log messages to the logfile, or the thread is stopping
                // Don't perform this check if fileout is disabled.
                if (((CurrentTime() - start_ns > _thread_write_period_ns) || stop || flush) && _fileout_enabled) {
                    start_ns = CurrentTime();

                    std::ofstream *log_file;

                    std::string filepath = "tmp.txt";
                    if (_filepath_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                        filepath = _filepath;
                        _filepath_sem.release();
                    }

                    log_file = new std::ofstream(filepath, std::ios::app); // try to open and append to file
                    if (!(*log_file)) { // if file DNE, create new file
                        delete log_file;
                        log_file = new std::ofstream(filepath);
                    }

                    while (!_filestream_queue.empty()) {
                        std::tuple<std::string, std::string, SEVERITY> item = _filestream_queue.front();
                        _filestream_queue.pop();

                        std::stringstream ss;
                        ss << std::get<1>(item) << severityToString[std::get<2>(item)] << ": " << std::get<0>(item) << std::endl;
                        (*log_file) << ss.str();
                    }
                    log_file->close();
                    delete log_file;
                }

                if (_filestream_queue.empty()){
                    if (_flush_sem.try_acquire()){
                        _flush = false;
                        _flush_sem.release();
                        flush = false;
                    }
                }
            }
        }

        bool _stop = false;
        bool _flush = false;
        bool _terminate = false;
        std::binary_semaphore _stop_sem{1};
        std::binary_semaphore _flush_sem{1};

        std::thread *_worker_t;
        std::binary_semaphore _ostream_sem{1};
        std::binary_semaphore _ostream_data_present_sem{0};

        std::queue<std::tuple<std::string, std::string, SEVERITY>> _ostream_queue;

        std::queue<std::tuple<std::string, std::string, SEVERITY>> _filestream_queue;
        ulong _thread_write_period_ns = 5 * 1.0e6; // write to file every 5 seconds
        SEVERITY _verbosity;
        bool _stdout_enabled = true;
        bool _fileout_enabled = true;

        std::binary_semaphore _filepath_sem{1};
        std::string _filepath;
    };
}