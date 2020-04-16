#!/usr/bin/env bash

sudo apt-get install -y libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler
sudo apt-get install -y --no-install-recommends libboost-all-dev
sudo apt-get install -y libopenblas-dev
sudo apt-get install -y cmake libyaml-cpp-dev libeigen3-dev liblmdb-dev libgoogle-glog-dev
echo 'Clone the repository from https://gitlab.com/bacox/edgecaffe.git'

echo 'Run protoc on source proto file in caffe!'