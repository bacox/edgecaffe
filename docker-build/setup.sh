#!/bin/bash
apt-get update
# Other dependencies
apt-get install -y  build-essential libssl-dev wget gcc-8 g++-8 libopenblas-dev libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev libboost-all-dev protobuf-compiler libgflags-dev libgoogle-glog-dev liblmdb-dev python3-dev libatlas-base-dev doxygen libyaml-cpp-dev python3-pip cgroup-bin
apt-get install -y --no-install-recommends libboost-all-dev
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
update-alternatives --config gcc
apt install python3
