#pragma once

#include <string>
#include <memory>
#include <thread>
#include <cstdlib>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "mongoose.h"

#include "Worker.h"


class WebServerWorker: public Worker {
public:

    WebServerWorker() = delete;
    ~WebServerWorker() = default;

    explicit WebServerWorker(unsigned short port);

    bool RegisterMatFunc(const std::function<cv::Mat()>& mat_func);

protected:

    void Init() override;
    void Execute() override;


private:

//    static void EventHandler(mg_connection* conn, int ev, void* ev_data, void* fn_data);
    void StartStreaming(mg_connection* conn);

    unsigned short _port;
    mg_mgr _mgr;
    mg_connection* _connection;

    std::binary_semaphore _frame_sem{1};
    std::vector<uchar> _frame_buf;

    std::vector<std::function<cv::Mat()>> _mat_funcs;

};