#!/bin/bash

# Corey Knutson, 12/2/2021

bash run-common.sh "mkdir -p build && cd build && cmake -DENABLE_TESTS=ON .. && make && ./runTests;"
