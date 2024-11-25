#pragma once

#include <string>
#include <memory>
#include <thread>
#include <cstdlib>

#include <opencv2/opencv.hpp>

#include "mongoose.h"

#include "Worker.h"

/**
 * @brief The threaded webserver which serves a simple MJPEG stream of all annotated camera images to clients. Runs on t
 * he host machine at 0.0.0.0:8080.
 */
class WebServerWorker: public Worker {
public:
    /**
     * @brief No copy constructor.
     */
    WebServerWorker (WebServerWorker const&) = delete; // delete copy constructor
    /**
     * @brief Setup the worker thread and instantiate the Mongoose server manager.
     * @param port The port that the webserver binds to.
     */
    explicit WebServerWorker(unsigned short port);
    /**
     *
     */
    ~WebServerWorker();
    /**
     * @brief Register the callback function which grabs the latest annotated camera image. All registered functions will
     * be called once an Execute cycle to acquire new camera data.
     * @param mat_func The callback function to execute to acquire fresh camera images.
     * @return true if registration succeeds, false otherwise.
     */
    bool RegisterMatFunc(const std::function<cv::Mat()>& mat_func);

protected:
    /**
     * @brief Start the webserver and setup the callback function to handle http requests. If the webserver fails to start,
     * wait 10 seconds then restart the worker.
     */
    void Init() override;
    /**
     * @brief Periodically get new camera images from each callback function. Only camera images of the same dimensions will
     * be displayed. Check for new connections to the webserver, then send the image as one piece of the MJPEG stream to the
     * client.
     */
    void Execute() override;

private:

    unsigned short _port = 8080;
    mg_mgr _mgr;
    mg_connection* _connection;

    std::vector<std::function<cv::Mat()>> _mat_funcs;

};