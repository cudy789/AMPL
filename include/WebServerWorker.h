#pragma once

#include <string>
#include <memory>
#include <thread>
#include <cstdlib>

#include <opencv2/opencv.hpp>

#include "mongoose.h"

#include "Worker.h"


class WebServerWorker: public Worker {
public:
    WebServerWorker (WebServerWorker const&) = delete; // delete copy constructor
    explicit WebServerWorker(unsigned short port);

    ~WebServerWorker() = default;

    bool RegisterMatFunc(const std::function<cv::Mat()>& mat_func);

protected:

    void Init() override;
    void Execute() override;

private:

    unsigned short _port = 8080;
    mg_mgr _mgr;

    std::vector<std::function<cv::Mat()>> _mat_funcs;

};