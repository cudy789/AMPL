#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <utility>
#include <thread>

#include "TimeUtils.h"

namespace AppLogger {

    /**
     * @brief Logger severity levels, ordered from lowest to highest severity.
     */
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

/**
 * @brief A singleton threaded logger class. Logs to stdout and a logfile with timestamps and varying levels of
 * severity. Non-blocking and high performant.
 */
    class Logger {
    public:
        /**
         * @brief No copy constructor allowed.
         */
        Logger (Logger const&) = delete;

        /**
         * @brief No assignment operator allowed.
         */
        void operator=(Logger const&) = delete;

        /**
         * @brief Log a message to stdout and the log file.
         * @param value The string to log.
         * @param level The severity of the message, defaults to INFO.
         * @return
         */
        static bool Log(const std::string& value, SEVERITY level = SEVERITY::INFO) {
            return GetInstance().log(value, level);
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
         * @brief Check if  logging to stdout.
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
         * @brief Get the singleton logger instance. If no instance exists, create a new static instance.
         * @return The singleton logger instance.
         */
        static Logger& GetInstance() {
            static Logger instance; // instantiated on first call, guaranteed to be destroyed
            return instance;
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

        /**
         * @brief Create the logger and its worker thread.
         */
        Logger() {
            _worker_t = new std::thread([this]() { this->Run(); });
            _filepath = "./ampl_log.txt";
            _verbosity = INFO;
            _stdout_enabled.store(true);
            _fileout_enabled.store(true);
        };
        /**
         * @brief Gracefully stop logging on destruction.
         */
        virtual ~Logger() {
            _stop_sem.acquire();
            if (!_stop){
                _stop_sem.release();
                Close();
            } else{
                _stop_sem.release();
            }
            delete _worker_t;
        }

        virtual void setStdOut(bool enabled){
            _stdout_enabled.store(enabled);
        }
        virtual bool getStdOut(){
            return _stdout_enabled.load();
        }
        virtual void setFileout(bool enabled){
            _fileout_enabled.store(enabled);
        }
        virtual bool getFileout(){
            return _fileout_enabled.load();
        }
        virtual void setVerbosity(SEVERITY verbosity){
            _verbosity = verbosity;
        }
        virtual std::string getFilepath(){
            if (_filepath_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                std::string filepath = _filepath;
                _filepath_sem.release();
                return filepath;
            }
            return "";
        }

        virtual bool setFilepath(const std::string& filepath){
            if (_filepath_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                _filepath = filepath;
                _filepath_sem.release();
                return true;
            }
            return false;
        }

        virtual bool stop(bool terminate=false) {
            if (_stop_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(2000))) {
                _stop = true;
                if (!_terminate) _terminate=terminate;
                _stop_sem.release();
                _ostream_data_present_sem.release();
                _worker_t->join();
                return true;
            } else {
                _ostream_data_present_sem.release();
                return false;
            }
        }

        virtual bool flush(){
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
        virtual std::stringstream messageFormatHelper(const std::tuple<std::string, SEVERITY>& data_tuple){
            std::stringstream ss;
            ss << "[" +  datetime_ms() + "] " << severityToColorString[std::get<1>(data_tuple)] << ": " << std::get<0>(data_tuple) << std::endl;
            return ss;
        }

        /**
         * @brief The worker thread of the logger. Waits until the log function notifies this thread new data is available,
         * then processes the available data to stdout, and occasionally flushes data to the logfile. The thread also
         * checks if the logger is shutting down and if data should be flushed immediately.
         */
        virtual void Run() {
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
                    std::tuple<std::string, SEVERITY> item = _ostream_queue.front();
                    if (_ostream_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(1000))){
                        _ostream_queue.pop();
                        _ostream_sem.release();
                        if (_stdout_enabled.load()){
                            std::stringstream ss = messageFormatHelper(item);
                            std::cout << ss.str();
                        }
                        if (_fileout_enabled.load()){
                            _filestream_queue.push(item);
                        }
                    }

                }

                // Check to see if enough time has passed and we should log messages to the logfile, or the thread is stopping
                // Don't perform this check if fileout is disabled.
                if (((CurrentTime() - start_ns > _thread_write_period_ns) || stop || flush) && _fileout_enabled.load()) {
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
                        std::tuple<std::string, SEVERITY> item = _filestream_queue.front();
                        _filestream_queue.pop();

                        std::stringstream ss = messageFormatHelper(item);
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

        std::queue<std::tuple<std::string, SEVERITY>> _ostream_queue;

        std::queue<std::tuple<std::string, SEVERITY>> _filestream_queue;
        ulong _thread_write_period_ns = 5 * 1.0e6; // write to file every 5 seconds
        SEVERITY _verbosity;

        std::atomic<bool> _stdout_enabled;
//        bool _stdout_enabled = true;
        std::atomic<bool> _fileout_enabled;
//        bool _fileout_enabled = true;

        std::binary_semaphore _filepath_sem{1};
        std::string _filepath;

        virtual bool log(const std::string& value, SEVERITY level = SEVERITY::INFO) {
            if (_stop) return false;
            if (level >= _verbosity){
                std::string date;
                if (_ostream_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(200))) {
                    _ostream_queue.push({value, level});
                    _ostream_sem.release();
                    _ostream_data_present_sem.release(); // Notify thread that data is available
                    return true;
                } else{
                    return false;
                }
            } else{
                return false;
            }
        }
    };
}