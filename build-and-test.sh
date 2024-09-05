#!/bin/bash

# Corey Knutson, 12/2/2021

GTESTFILTER=""
if [ -n "$1" ]; then
    GTESTFILTER="--gtest_filter=$1"
fi

echo "./runTests $GTESTFILTER"

bash run-common.sh "mkdir -p build && cd build && cmake -DENABLE_TESTS=ON .. && make && ./runTests $GTESTFILTER;"
