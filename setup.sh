#!/bin/bash
sudo apt update
# Other dependencies
sudo apt install -y  build-essential libssl-dev wget gcc-8 g++-8 libopenblas-dev libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev libboost-all-dev protobuf-compiler libgflags-dev libgoogle-glog-dev liblmdb-dev python3-dev libatlas-base-dev doxygen libyaml-cpp-dev python3-pip
sudo apt install -y cgroup-bin
sudo apt install -y cgroup-tools
sudo apt install -y --no-install-recommends libboost-all-dev
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
sudo update-alternatives --config gcc
pip3 install cython numpy
pip3 install protobuf pyyaml pandas

# Install cmake 3.19
wget https://cmake.org/files/v3.19/cmake-3.19.3.tar.gz
tar -xf ./cmake-3.19.3.tar.gz
cd cmake-3.19.3
./bootstrap
make -j2
sudo make install
cd ..
rm -rf cmake-3.19.3

# Generate probobuf files
protoc caffe/src/caffe/proto/caffe.proto --cpp_out=.
mkdir -p caffe/include/caffe/proto
mv caffe/src/caffe/proto/caffe.pb.h caffe/include/caffe/proto