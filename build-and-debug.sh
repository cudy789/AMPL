#!/bin/bash

# Corey Knutson, 12/2/2021

# Do a grep exclude at the end to remove error messages of Corrupt JPEG data. Cause by a bug in OpenCV's libjpeg library
# For more details... https://github.com/opencv/opencv/issues/9477
bash run-common.sh "mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j4 && gdb ./apriltag_localization;"
