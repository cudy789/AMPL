#pragma once

#include <thread>
#include <utility>
#include <bits/stdc++.h>

#include "Logger.h"


class Worker{

public:
    Worker(std::string thread_name, int debug_v=AppLogger::DEBUG){
        _thread_name = std::move(thread_name);
        _debug_v = debug_v;
    }
    ~Worker(){
        delete _t_worker;
    }
    void Start(){
        _t_worker = new std::thread([this]() {this->Run();});
    };
    bool Stop(){
        if (_stop_sem.try_acquire_for(std::chrono::duration<ulong, std::milli>(2000))){
            _stop = true;
            _stop_sem.release();
            return true;
        } else{
            return false;
        }
    };
    void join(){
        if (_t_worker){
            _t_worker->join();
        }
    };

protected:
    virtual void RunOnce(){};
    std::thread* _t_worker;

private:
    bool _stop = false;
    std::string _thread_name;
    std::binary_semaphore _stop_sem{1};
    int _debug_v = 0;


    void Run(){
        std::vector<ulong> runtimes;
        ulong start_ns, end_ns;


        while (true){
            start_ns = CurrentTime();
            if (_stop_sem.try_acquire()){
                if (_stop){
                    break;
                }
                _stop_sem.release();
                RunOnce();
            }
            end_ns = CurrentTime();
            runtimes.push_back(end_ns - start_ns);
            if (runtimes.size() == 500){
                if (_debug_v <= AppLogger::DEBUG){
                    std::cout << "Average " + _thread_name + " execution time: " << (std::accumulate(runtimes.begin(), runtimes.end(), 0.0) / (1.0e6 * runtimes.size())) << "ms" << std::endl;
                    std::cout << "\t Max: " << *std::max_element(runtimes.begin(), runtimes.end()) / 1.0e6 << "ms" << std::endl;
                }
                runtimes.clear();
            }


        }
    };



};