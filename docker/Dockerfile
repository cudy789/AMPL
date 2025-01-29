# Corey Knutson, 8/20/2024
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y vim inetutils-ping net-tools git screen usbutils python3-pip g++ cmake make  \
    libgtk2.0-dev pkg-config sudo ffmpeg libavformat-dev libavcodec-dev libswscale-dev gdb valgrind

# ============ Install OpenCV ============ 

RUN mkdir /opencv_install
WORKDIR /opencv_install

RUN git clone https://github.com/opencv/opencv.git
RUN mkdir build
WORKDIR build
RUN cmake -D WITH_FFMPEG=ON ../opencv
RUN make -j4
RUN make install

# ============ Install dependencies for NetworkTables ============
# Java JRE/JDK
RUN apt-get update && apt-get install -y default-jre default-jdk

# ============ Install Doxygen & Sphinx ============
RUN apt-get update && apt-get install -y doxygen python3-sphinx python3-breathe
RUN pip install sphinx_rtd_theme sphinx-tabs pexpect

# ============ Install Pybullet for simulation video generation ============

RUN pip install numpy==1.23.1 scipy pybullet pyyaml

# ============ Build and run MAPLE ============
ARG CACHE_BUST

RUN mkdir /app
WORKDIR /app
RUN git clone https://github.com/cudy789/MAPLE.git
WORKDIR MAPLE
RUN mkdir -p build
WORKDIR build
RUN cmake .. && make -j4