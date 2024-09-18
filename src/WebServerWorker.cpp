#include "WebServerWorker.h"

WebServerWorker::WebServerWorker(unsigned short port) :
        Worker{"Webserver worker", 50.0},  // Call Worker constructor
        _port(port),
        _io_service(),
        _acceptor(_io_service, tcp::endpoint(tcp::v4(), _port)),
        _err(),
        _socket(_io_service)
{
    AppLogger::Logger::Log("Starting webserver");
}

bool WebServerWorker::RegisterMatFunc(const std::function<cv::Mat()>& mat_func){
    _mat_funcs.emplace_back(mat_func);
    return true;
}

// TODO webserver doesn't handle multiple connections, only the first one that makes a connection
void WebServerWorker::Init() {

    _acceptor.accept(_socket);

    boost::beast::flat_buffer request_buffer;

    http::request<http::string_body> req;
    http::read(_socket, request_buffer, req, _err);
    if(_err){
        std::cerr << "read: " << _err.message() << "\n";
    }

    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "multipart/x-mixed-replace; boundary=frame");
    res.keep_alive();
    http::response_serializer<http::empty_body> sr{res};
    http::write_header(_socket, sr);
    AppLogger::Logger::Log("Client connected to webserver, starting stream");
}

void WebServerWorker::Execute() {
    if (!_mat_funcs.empty()){
        std::vector<uchar> buffer;
        cv::Mat merged_frame = _mat_funcs[0]();

        for (int i=1; i<_mat_funcs.size(); i++){
            cv::hconcat(merged_frame, _mat_funcs[i](), merged_frame);
        }

        if (!merged_frame.empty()) {
            cv::imencode(".jpg", merged_frame, buffer, std::vector<int>{cv::IMWRITE_JPEG_QUALITY, 95});

            auto const size = buffer.size();

            std::string message{"\r\n--frame\r\nContent-Type: image/jpeg\r\nContent-Length: "};
            message += std::to_string(size);
            message += "\r\n\r\n";
            auto bytesSent = _socket.send(boost::asio::buffer(message), 0, _err);
            if( !bytesSent )
            {
                _socket.close();
                Init();
            }
            bytesSent = _socket.send(boost::asio::buffer(buffer), 0, _err);
            if( !bytesSent )
            {
                _socket.close();
                Init();
            }
        }

    } else {
        AppLogger::Logger::Log("Webserver has no camera streams", AppLogger::SEVERITY::WARNING);
    }

}