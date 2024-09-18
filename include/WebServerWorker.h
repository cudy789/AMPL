#pragma once

#include <string>
#include <memory>
#include <thread>
#include <cstdlib>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>

#include "Worker.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class WebServerWorker: public Worker {
public:

    WebServerWorker() = delete;
    ~WebServerWorker() = default;

    WebServerWorker(unsigned short port);

    bool RegisterMatFunc(const std::function<cv::Mat()>& mat_func);

protected:

    void Init() override;
    void Execute() override;


private:
    unsigned short _port;

    boost::asio::io_context _io_service;
    tcp::acceptor _acceptor;
    boost::system::error_code _err;
    tcp::socket _socket;

    std::vector<std::function<cv::Mat()>> _mat_funcs;

};