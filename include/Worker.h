#pragma once

#include <thread>
#include <utility>
#include <bits/stdc++.h>

#include "Logger.h"
#include "TimeUtils.h"

/***
 * Worker lifecycle
 *
 * Instantiation: create worker object with desired execution frequency, thread name (for debug), and logging verbosity.
 * Start: Called once. Calls run function to begin thread execution.
 * Stop: Called once. Ends thread execution; blocks until the thread has stopped. A stopped worker cannot be restarted, a new object must be created.
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

    explicit Worker(std::string thread_name, double execution_freq = 100.0, AppLogger::SEVERITY debug_verbosity=AppLogger::DEBUG){
        _exec_freq = execution_freq;
        _thread_name = std::move(thread_name);
        _debug_v = debug_verbosity;
        _t_worker = nullptr;
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

    void Start(){
        _t_worker = new std::thread([this]() {this->run();});
    };
    bool Stop(){
        if (_stop_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(3000))){
            _stop = true;
            _stop_sem.release();
            Join();
            return true;
        } else{
            return false;
        }
    };

    void Join(){
        if (_t_worker){
            _t_worker->join();
        }
    };

protected:
    virtual void Execute() {};
    virtual void Init() {};
    std::thread* _t_worker;

private:
    bool _stop = false;
    std::string _thread_name;
    std::binary_semaphore _stop_sem{1};
    int _debug_v = 0;
    double _exec_freq;

    /***
     * Run the execute function at a maximum rate of _exec_freq. No timeout if execution cannot match desired freqency.
     * Log the execution time average and max.
     *
     */
    void run(){
        std::vector<ulong> runtimes;
        ulong last_loop_ns, last_log_time_ns, current_duration_ns;
        last_loop_ns = CurrentTime();
        last_log_time_ns = last_loop_ns;

        Init();

        while (true){

            if (_stop_sem.try_acquire()){
                if (_stop){
                    _stop_sem.release();
                    break;
                }
                _stop_sem.release();

                current_duration_ns = CurrentTime() - last_loop_ns;
                if (current_duration_ns >= ((1.0/_exec_freq) * 1.0e9)){
                    last_loop_ns = CurrentTime();
                    Execute();

                    runtimes.push_back(current_duration_ns);
                } else {
                    int sleep_duration_ns = (int) (0.66 * (((1.0 / _exec_freq) * 1.0e9) - current_duration_ns));
                    std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_duration_ns));
                }
            }

            if ((CurrentTime() - last_log_time_ns) > 1.0e9){
                if (_debug_v <= AppLogger::DEBUG){
                    AppLogger::Logger::Log("Average " + _thread_name + " execution time: " + std::to_string(std::accumulate(runtimes.begin(), runtimes.end(), 0.0) / (1.0e6 * runtimes.size())) + "ms", AppLogger::DEBUG);
                    AppLogger::Logger::Log("\t Max: " + std::to_string(*std::max_element(runtimes.begin(), runtimes.end()) / 1.0e6) + "ms", AppLogger::DEBUG);
                }
                last_log_time_ns = CurrentTime();
                runtimes.clear();
            }

        }
    };

};