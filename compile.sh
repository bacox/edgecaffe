#!/bin/bash
mkdir -p cmake-build-release
cd cmake-build-release || exit
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
cd ..