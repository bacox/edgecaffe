#!/usr/bin/env bash
sudo apt install -y python3-opencv
sudo apt-get install -y libopenblas-dev # OpenBLAS

sudo apt-get install -y libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev libboost-all-dev protobuf-compiler
sudo apt-get install -y --no-install-recommends libboost-all-dev
sudo apt-get install -y libgflags-dev libgoogle-glog-dev liblmdb-dev
pip3 install protobuf
sudo apt-get install -y python3-dev
sudo apt-get install -y libatlas-base-dev doxygen libyaml-cpp-dev
echo 'Clone the repository from https://gitlab.com/bacox/edgecaffe.git'

echo 'Run protoc on source proto file in caffe!'