#include <gtest/gtest.h>
#include <iostream>

#include "common.h"
//#include "WebServer.h"


#include <iostream>
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

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

TEST(WebServer, HelloWorld){
    SetupLogger("WebServer_HelloWorld");


    cv::Mat frame;
    std::vector<uchar> buffer;
    cv::Mat grayframe;

    try{
        boost::asio::io_context io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1112));
        boost::system::error_code err;
        tcp::socket socket(io_service);
        acceptor.accept(socket);

        boost::beast::flat_buffer request_buffer;

        cv::VideoCapture cap(0);

        http::request<http::string_body> req;
        http::read(socket, request_buffer, req, err);
        if(err){
            std::cerr << "read: " << err.message() << "\n";
        }

        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "multipart/x-mixed-replace; boundary=frame");
        res.keep_alive();
        http::response_serializer<http::empty_body> sr{res};
        http::write_header(socket, sr);

        while(cap.isOpened()){
            cap >> frame;
            cv::cvtColor(frame, grayframe, cv::COLOR_BGR2GRAY);
            cv::imencode(".jpg", grayframe, buffer, std::vector<int> {cv::IMWRITE_JPEG_QUALITY, 95});

            auto const size = buffer.size();

            // do not use http::response<>
            // hack: write to socket the multipart message
            std::string message { "\r\n--frame\r\nContent-Type: image/jpeg\r\nContent-Length: " };
            message += std::to_string( size );
            message += "\r\n\r\n";
            auto bytesSent = socket.send( boost::asio::buffer( message ), 0, err );
            if( !bytesSent )
            {
                break;
            }
            bytesSent = socket.send( boost::asio::buffer( buffer ), 0, err );
            if( !bytesSent )
            {
                break;
            }
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
//        return EXIT_FAILURE;
    }
//    return 0;


    AppLogger::Logger::Log("Stopped webserver");
    AppLogger::Logger::Flush();

}