#include "WebServerWorker.h"

WebServerWorker::WebServerWorker(unsigned short port) :
        Worker{"Webserver worker", true, 50.0},  // Call Worker constructor
        _port(port),
        _mgr(mg_mgr())
{}

WebServerWorker::~WebServerWorker() {
    delete _connection;
    delete _viewer_connection;
    delete _ws_connection;
}

bool WebServerWorker::RegisterMatFunc(const std::function<cv::Mat()>& mat_func) {
    _mat_funcs.emplace_back(mat_func);
    return true;
}

bool WebServerWorker::RegisterRobotPoseFunc(const std::function<RobotPose()>& pose_func) {
    _robot_pose_func = pose_func;
    return true;
}

void WebServerWorker::Init() {
    mg_mgr_init(&_mgr);

    std::string address = "http://0.0.0.0:" + std::to_string(_port);
    _connection =
        mg_http_listen(&_mgr, address.c_str(),
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

    std::string viewer_address = "http://0.0.0.0:" + std::to_string(_port+1);

    _viewer_connection =
        mg_http_listen(&_mgr, viewer_address.c_str(),
           [](mg_connection *conn, int ev, void *ev_data) {
               if (ev == MG_EV_HTTP_MSG) {
                   struct mg_http_message *hm = (struct mg_http_message *) ev_data;
                   struct mg_http_serve_opts opts = { .root_dir = "./" };

                   mg_http_serve_file(conn, hm, "viewer.html", &opts);
                   AppLogger::Logger::Log("Client connected to viewer");
               }
           },
           this);

    std::string ws_address = "http://0.0.0.0:" + std::to_string(_port+2);

    _ws_connection =
            mg_http_listen(&_mgr, ws_address.c_str(),
                           [](mg_connection *conn, int ev, void *ev_data) {
                               if (ev == MG_EV_HTTP_MSG) {
                                   AppLogger::Logger::Log("Get http request on websocket port, upgrading to ws");
                                   struct mg_http_message *hm = (struct mg_http_message *) ev_data;
                                   mg_ws_upgrade(conn, hm, NULL);
                               } else if (ev == MG_EV_WS_OPEN){
                                   AppLogger::Logger::Log("Websocket connected");
                                   conn->data[0] = 'W';
                               }
                           },
                           this);

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
                if (conn->data[0] == 'S'){
                    mg_printf(conn,
                              "--frame\r\n"
                              "Content-Type: image/jpeg\r\n"
                              "Content-Length: %lu\r\n\r\n",
                              buf.size());
                    mg_send(conn, buf.data(), buf.size());
                    mg_printf(conn, "\r\n", 2);
                } else if (conn->data[0] == 'W'){
                    RobotPose latest_pose = _robot_pose_func();
                    double x = latest_pose.global.T[0];
                    double y = latest_pose.global.T[1];
                    double z = latest_pose.global.T[2];
                    Eigen::Vector3d rpy = RotationMatrixToRPY(latest_pose.global.R);

                    std::string test_data =
                            "{\"x\": " + to_string(x) +
                            ",\"y\": " + to_string(y) +
                            ",\"z\": " + to_string(z) +
                            ",\"roll\": " + to_string(rpy[0]) +
                            ",\"pitch\": " + to_string(rpy[1]) +
                            ",\"yaw\": " + to_string(rpy[2]) + "}";

                    mg_ws_send(conn, test_data.c_str(), test_data.size(), WEBSOCKET_OP_TEXT);
                }
            }
        } else {
            AppLogger::Logger::Log("Webserver merged frame is empty", AppLogger::SEVERITY::DEBUG);
        }

    } else {
        AppLogger::Logger::Log("Webserver has no camera streams", AppLogger::SEVERITY::WARNING);
    }

    mg_mgr_poll(&_mgr, 0);
}