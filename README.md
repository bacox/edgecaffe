# EdgeCaffe

## Build

### Install dependencies

OpenCV

```bash
sudo apt install python3-opencv
```

Blas

```bash
sudo apt-get install libopenblas-dev # OpenBLAS
```

Other

```bash
# Other dependencies
$ sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler
$ sudo apt-get install — no-install-recommends libboost-all-dev
$ sudo apt-get install libgflags-dev libgoogle-glog-dev liblmdb-dev
$ sudo pip3 install protobuf
$ sudo apt-get install the python3-dev
```

### Compile

```
mkdir build
cd build
cmake ..
build py_edge_caffe
```



## Run

### Python

Add python directory (of the EdgeCaffe project) to python path

```bash
export PYTHONPATH=path/to/python_dir:$PYTHONPATH
```

Import EdgeCaffe into python

```bash
$ python3
>>> import caffe
```