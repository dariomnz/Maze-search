#!/bin/bash
set -e


cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=main > /dev/null
cmake --build build --config Debug --parallel 8
cp ./build/main ./main

if [ "$1" = "run" ]; then
    echo "-----------Run RayText-----------"
    ./main
fi