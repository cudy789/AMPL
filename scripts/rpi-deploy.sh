#!/bin/bash

# Corey Knutson, 12/2/2021

D_USER=pi
D_HOST=apriltagpi.local
D_DIR=/home/pi/multicam-apriltag-localization/build/apriltag_localization

echo "Compiling for arm64 and uploading to $D_USER@$D_HOST:$D_DIR"

#rm build/CMakeCache.txt

ARM=1 bash run-common.sh "mkdir -p build && cd build && cmake .. && make -j4;"

scp build/apriltag_localization $D_USER@$D_HOST:$D_DIR
