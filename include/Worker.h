#pragma once

#include <thread>
#include <utility>
#include <bits/stdc++.h>
#include <stdexcept>

#include "Logger.h"
#include "TimeUtils.h"

// TODO switch to std::atomic 's for bools

/***
 * Worker lifecycle
 *
 * Instantiation: create worker object with desired execution frequency, thread name (for debug), and logging verbosity.
 * Start: Called once. Calls run function to begin thread execution.
 * Stop: Called once. Ends thread execution; blocks until the thread has stopped. If param interrupted=false,
 * worker is restarted A stopped worker cannot be restarted, a new object must be created.
 *
 * Init: Called once when the worker is told to begin execution (after Start is called)
 * Execute: Called periodically at a maximum rate of _exec_freq Hz until Stop() is called.
 * Join: Blocks until the thread object has finshed.
 *
 * run: Private member function that handles Execute timing and interrupts
 *
 */
class Worker{

public:
    Worker() : Worker("THREADNAME"){}

    explicit Worker(std::string thread_name){
        _thread_name = thread_name;
        _t_worker = nullptr;
    }
    explicit Worker(std::string thread_name, bool stay_alive): Worker(std::move(thread_name)) {
        _stay_alive = stay_alive;
    }

    explicit Worker(std::string thread_name, bool stay_alive, double execution_freq): Worker(std::move(thread_name), stay_alive){
        _exec_freq = execution_freq;
    }

    explicit Worker(std::string thread_name, bool stay_alive, double execution_freq, AppLogger::SEVERITY debug_verbosity): Worker(std::move(thread_name), stay_alive, execution_freq){
        _debug_v = debug_verbosity;
    }

    Worker(const Worker& o){
        _exec_freq = o._exec_freq;
        _thread_name = o._thread_name;
        _debug_v = o._debug_v;
        _t_worker = nullptr;
    }

    virtual ~Worker(){
        if (_stop_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(3000))){
            if (!_stop) Stop();
            _stop_sem.release();
        }
        delete _t_worker;
    }

    void SetStayAlive(bool stay_alive){
        _stay_alive_sem.acquire();
        _stay_alive = stay_alive;
        _stay_alive_sem.release();
    }

    bool GetStayAlive(){
        bool stay_alive;
        _stay_alive_sem.acquire();
        stay_alive = _stay_alive;
        _stay_alive_sem.release();
        return stay_alive;
    }

    bool Stopped(){
        bool stop;
        _stop_sem.acquire();
        stop = _stop;
        _stop_sem.release();
        return stop;
    }

    const std::string& GetName(){
        return _thread_name;
    }

    void Start(){
        _t_worker = new std::thread([this]() {this->run();});
    };
    bool Stop(bool interrupted=true){
        _stop_sem.acquire();
        _interrupted_sem.acquire();
        _stop = true;
        if (!_interrupted) _interrupted = interrupted; // only change interrupted flag if not already interrupted
        _stay_alive_sem.acquire();
        if (_stay_alive && !interrupted){
            _stay_alive_sem.release();
            _stop_sem.release();
            _interrupted_sem.release();
            return false;
        }
        _stop_sem.release();
        _interrupted_sem.release();
        _stay_alive_sem.release();
        Join();
        return true;
    };

    void Join(){
        if (_t_worker){
            _t_worker->join();
        }
    };

    double GetExecutionFreq(){
        double ret_exec_freq;
        if (_exec_freq_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(100))){
            ret_exec_freq = _measure_exec_freq;
            _exec_freq_sem.release();
        }

        return ret_exec_freq;
    }

protected:
    virtual void Execute() {};
    virtual void Init() {};
    virtual void Finish() {
        AppLogger::Logger::Log(_thread_name + " exited");
    };
    std::thread* _t_worker{};

private:
    bool _stop = false;
    std::binary_semaphore _stop_sem{1};
    bool _stay_alive = true;
    std::binary_semaphore _stay_alive_sem{1};
    bool _interrupted = false;
    std::binary_semaphore _interrupted_sem{1};
    double _exec_freq = 100.0;

    double _measure_exec_freq{};
    std::binary_semaphore _exec_freq_sem{1};

    std::string _thread_name;
    int _debug_v = AppLogger::SEVERITY::INFO;

    /***
     * Run the execute function at a maximum rate of _exec_freq. No timeout if execution cannot match desired frequency.
     * Log the execution time average and max.
     *
     */
    void run(){
        while (true) {
            try {

                std::vector<ulong> runtimes;
                ulong last_loop_ns, last_log_time_ns, current_duration_ns;
                last_loop_ns = CurrentTime();
                last_log_time_ns = last_loop_ns;

                Init();

                while (true) {

                    if (_stop_sem.try_acquire()) {
                        if (_stop) {
                            _stop_sem.release();
                            break;
                        }
                        _stop_sem.release();

                        current_duration_ns = CurrentTime() - last_loop_ns;
                        if (current_duration_ns >= ((1.0 / _exec_freq) * 1.0e9)) {
                            last_loop_ns = CurrentTime();
                            Execute();

                            runtimes.push_back(current_duration_ns);
                        } else {
                            int sleep_duration_ns = (int) (0.66 * (((1.0 / _exec_freq) * 1.0e9) - current_duration_ns));
                            std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_duration_ns));
                        }
                    }

                    if ((CurrentTime() - last_log_time_ns) > 1.0e9) {
                        double avg_exec_ms =
                                std::accumulate(runtimes.begin(), runtimes.end(), 0.0) / (1.0e6 * runtimes.size());
                        if (_exec_freq_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(10))) {
                            _measure_exec_freq = 1.0 / (avg_exec_ms / 1.0e3);
                            _exec_freq_sem.release();
                        }
                        if (_debug_v <= AppLogger::DEBUG) {
                            AppLogger::Logger::Log(
                                    "Average " + _thread_name + " execution ms: " + std::to_string(avg_exec_ms),
                                    AppLogger::DEBUG);
                            AppLogger::Logger::Log("\t Max: " + std::to_string(
                                                           *std::max_element(runtimes.begin(), runtimes.end()) / 1.0e6) + "ms",
                                                   AppLogger::DEBUG);
                        }
                        last_log_time_ns = CurrentTime();
                        runtimes.clear();
                    }

                }
            } catch (const std::exception& e){
                AppLogger::Logger::Log(e.what());
            }

            _stay_alive_sem.acquire();
            _interrupted_sem.acquire();
            if (!_stay_alive || _interrupted) {
                _stay_alive_sem.release();
                _interrupted_sem.release();
                Finish();
                break;
            }

            _stay_alive_sem.release();
            _interrupted_sem.release();

            _stop_sem.acquire();
            _stop = false;
            _stop_sem.release();
            Finish();

            AppLogger::Logger::Log("Stay alive enabled, thread " + _thread_name + " will restart");
        }
    };

};