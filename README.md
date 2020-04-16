# EdgeCaffe

## Build

### Install dependencies

OpenCV

```bash
sudo apt install -y python3-opencv
```

Blas

```bash
sudo apt-get install -y libopenblas-dev # OpenBLAS
```

Other

```bash
# Other dependencies
sudo apt-get install -y libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler
sudo apt-get install -y — no-install-recommends libboost-all-dev
sudo apt-get install -y libgflags-dev libgoogle-glog-dev liblmdb-dev
sudo pip3 install -y protobuf
sudo apt-get install -y the python3-dev
```

### Compile

#### RunPipeline

```bash
mkdir build
cd build
cmake ..
build RunPipeline
```



#### EdgeCaffe Library

```bash
mkdir build
cd build
cmake ..
build EdgeCaffeCore
```



#### PyEdgeCaffe

```
mkdir build
cd build
cmake ..
build py_edge_caffe
```



#### Modelsplitter

```bash
mkdir build
cd build
cmake ..
build ModelSplitter
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

## Download models

The model files of the networks can be downloaded using the following link: https://bartcox.stackstorage.com/s/GmO0bKJb4JV5Qvd

The modelfiles needs to be placed in the networks folder in their respective networkfolder. For example: the file `age_net.caffemodel` needs to be placed as `networks/AgeNet/age_net.caffemodel`.

After placing the models, run `cmake ..` in the build folder to copy the model resources to the build directory.