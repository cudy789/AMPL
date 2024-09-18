# Corey Knutson, 8/20/2024
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y vim inetutils-ping net-tools git screen usbutils python3-pip g++ cmake make libgtk2.0-dev pkg-config sudo

# ============ Install OpenCV ============ 

RUN mkdir /opencv_install
WORKDIR /opencv_install

RUN git clone https://github.com/opencv/opencv.git
RUN mkdir build
WORKDIR build
RUN cmake WITH_JPEG=OFF ../opencv
RUN make -j4
RUN make install

# ============ Install Boost.Beast ============

RUN apt-get update && apt-get install -y libboost-all-dev libssl-dev
