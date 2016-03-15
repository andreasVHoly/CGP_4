#!/bin/sh

rm -r build
mkdir build
rm CMakeCache.txt
cd build
cmake -DCMAKE_CXX_COMPILER=g++ ..
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DBoost_NO_SYSTEM_PATHS=1 ..
cd build
make