#include "WebServerWorker.h"

WebServerWorker::WebServerWorker(unsigned short port) :
        Worker{"Webserver worker", true, 50.0},  // Call Worker constructor
        _port(port),
        _mgr(mg_mgr())
{}

bool WebServerWorker::RegisterMatFunc(const std::function<cv::Mat()>& mat_func) {
    _mat_funcs.emplace_back(mat_func);
    return true;
}

void WebServerWorker::Init() {
    mg_mgr_init(&_mgr);

    std::string address = "http://0.0.0.0:" + std::to_string(_port);
    mg_connection* _connection = mg_http_listen(&_mgr, address.c_str(),
                                 [](mg_connection *conn, int ev, void *ev_data) {
                                     if (ev == MG_EV_HTTP_MSG) {
                                         mg_printf(conn,
                                                   "HTTP/1.0 200 OK\r\n"
                                                   "Cache-Control: no-cache\r\n"
                                                   "Pragma: no-cache\r\nExpires: Thu, 01 Dec 1994 16:00:00 GMT\r\n"
                                                   "Content-Type: multipart/x-mixed-replace; boundary=--frame\r\n\r\n");
                                         conn->data[0] = 'S';
                                         AppLogger::Logger::Log("Client connected to webserver");
                                     }

                                 },
                                 this);
    if (_connection == nullptr) {
        AppLogger::Logger::Log("Failed to start Mongoose server on port " + std::to_string(_port), AppLogger::SEVERITY::WARNING);
        sleep(10);
        Stop(false);
    }
    AppLogger::Logger::Log("Starting webserver");
}

void WebServerWorker::Execute() {
    if (!_mat_funcs.empty()){
        cv::Mat merged_frame = _mat_funcs[0]();
        for (int i=1; i<_mat_funcs.size(); i++){
            const cv::Mat& new_frame = _mat_funcs[i]();
            if (merged_frame.empty()) merged_frame = new_frame;
            if (new_frame.empty()) continue;
            if (new_frame.rows != merged_frame.rows){// TODO allow multiple camera image sizes
                continue;
            }
            cv::hconcat(merged_frame, new_frame, merged_frame);
        }
        if (!merged_frame.empty()) {
            std::vector<uchar> buf;
            cv::imencode(".jpg", merged_frame, buf, std::vector<int>{cv::IMWRITE_JPEG_QUALITY, 85});

            mg_connection *conn;
            for (conn=_mgr.conns; conn !=nullptr; conn=conn->next){
                if (conn->data[0] != 'S') continue; // ignore non-stream connections
                mg_printf(conn,
                          "--frame\r\n"
                          "Content-Type: image/jpeg\r\n"
                          "Content-Length: %lu\r\n\r\n",
                          buf.size());
                mg_send(conn, buf.data(), buf.size());
                mg_printf(conn, "\r\n", 2);
            }
        } else {
            AppLogger::Logger::Log("Webserver merged frame is empty", AppLogger::SEVERITY::DEBUG);
        }

    } else {
        AppLogger::Logger::Log("Webserver has no camera streams", AppLogger::SEVERITY::WARNING);
    }

    mg_mgr_poll(&_mgr, 0);
}