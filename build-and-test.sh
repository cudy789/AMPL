#!/bin/bash

# Corey Knutson, 12/2/2021


echo "./runTests --gtest_filter=$1"

bash run-common.sh "mkdir -p build && cd build && cmake -DENABLE_TESTS=ON .. && make && ./runTests --gtest_filter=$1;"
