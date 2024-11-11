#include "WebServerWorker.h"

WebServerWorker::WebServerWorker(unsigned short port) :
        Worker{"Webserver worker", true, 50.0},  // Call Worker constructor
        _port(port)
{

}

bool WebServerWorker::RegisterMatFunc(const std::function<cv::Mat()>& mat_func){
    _mat_funcs.emplace_back(mat_func);
    return true;
}

// TODO webserver doesn't handle multiple connections, only the first one that makes a connection
void WebServerWorker::Init() {
    _mgr = mg_mgr();
    mg_mgr_init(&_mgr);

    std::string address = "http://0.0.0.0:" + std::to_string(_port);
    _connection = mg_http_listen(&_mgr, address.c_str(), [](mg_connection *conn, int ev, void *ev_data) {
        // Use the fn_data pointer to access the ImageStreamer instance
        WebServerWorker* streamer = static_cast<WebServerWorker*>(conn->fn_data);
        if (ev == MG_EV_HTTP_MSG) {
            streamer->StartStreaming(conn);
        }
    }, this);  // Pass 'this' as the fn_data argument
    if (_connection == nullptr) {
        AppLogger::Logger::Log("Failed to start Mongoose server on port " + std::to_string(_port), AppLogger::SEVERITY::WARNING);
        Stop(false);
    }
    AppLogger::Logger::Log("Starting webserver");

}

//void WebServerWorker::EventHandler(mg_connection* conn, int ev, void* ev_data, void* fn_data) {
//    if (ev == MG_EV_HTTP_MSG) {
//        WebServerWorker* streamer = static_cast<WebServerWorker*>(fn_data);
//        streamer->StartStreaming(conn);
//    }
//}

void WebServerWorker::StartStreaming(mg_connection* conn) {
    mg_printf(conn,
              "HTTP/1.0 200 OK\r\n"
              "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n");

    while (!Stopped() && conn->is_accepted){
        AppLogger::Logger::Log("in web server http response loop!");

        _frame_sem.acquire();
        if (_frame_buf.empty()) {
            std::cerr << "Error: No frame to stream." << std::endl;
            _frame_sem.release();
            continue;
        }
        std::vector<uchar> buf = _frame_buf;
//        std::string message{"\r\n--frame\r\nContent-Type: image/jpeg\r\nContent-Length: "};
//        message += std::to_string(size);
//        message += "\r\n\r\n";

        mg_printf(conn,
                  "--frame\r\n"
                  "Content-Type: image/jpeg\r\n"
                  "Content-Length: %d\r\n\r\n",
                  buf.size());
        mg_send(conn, buf.data(), buf.size());

        mg_printf(conn, "\r\n", 2);
        // Control the streaming rate
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Adjust frame rate as needed


    }
}

void WebServerWorker::Execute() {
    if (!_mat_funcs.empty()){
//        std::vector<uchar> buffer;
        cv::Mat merged_frame = _mat_funcs[0]();

        for (int i=1; i<_mat_funcs.size(); i++){
            cv::hconcat(merged_frame, _mat_funcs[i](), merged_frame);
        }

        if (!merged_frame.empty()) {
            _frame_sem.acquire();
            cv::imencode(".jpg", merged_frame, _frame_buf, std::vector<int>{cv::IMWRITE_JPEG_QUALITY, 85});
//            cv::imencode(".jpg", merged_frame, _frame_buf);
            _frame_sem.release();
        }

    } else {
        AppLogger::Logger::Log("Webserver has no camera streams", AppLogger::SEVERITY::WARNING);
    }

    mg_mgr_poll(&_mgr, 0);

}