# EdgeCaffe (Ubuntu 18.04)

The EdgeCaffe project aims to run the [Caffe Deep Learning framework](https://github.com/BVLC/caffe) on edge devices. The code has is targeted for Ubuntu 18.04 for both x86-64 and Raspberry Pi machines (ARMv8-A).

The main codebase is written in C++, although there are limited bindings to Python.

## Project organization

The project is divided in six sections:

* `./caffe`: This holds the original caffe code.
* `./Core`: The code extension to facilitate the partial loading and execution.
* `./networks`: The networks and their descriptive files. Each model to work needs 3 files:
  * `description.yml` file that describes the network for EdgeCaffe
  * `.prototxt` file that describes the network architecture
  * `.caffemodel` file that holds all the trained parameters
* `./tools`: The ModelSplitter tool
* `./analysis`: This folder is empty on initialization of the project but will hold the output data of executable `RunPipeline`.
* `./resources`: Holds the test data files.

## Install dependencies

Cmake (VERSION  >= 3.1.0)

```bash
sudo apt-get install -y cmake
```

GCC (VERSION >= 8)

```bash
sudo apt-get update
sudo apt-get install -y gcc-8 g++-8
# Make sure to set the priority
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
sudo update-alternatives --config gcc
```

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
sudo apt-get install -y —-no-install-recommends libboost-all-dev
sudo apt-get install -y libgflags-dev libgoogle-glog-dev liblmdb-dev
pip3 install protobuf
sudo apt-get install -y python3-dev
sudo apt-get install -y libatlas-base-dev doxygen libyaml-cpp-dev
```

## Compile (Laptop)

#### Generate protobuf bindings
This step should be run before compiling

```bash
protoc caffe/src/caffe/proto/caffe.proto --cpp_out=.
mkdir caffe/include/caffe/proto
mv caffe/src/caffe/proto/caffe.pb.h caffe/include/caffe/proto
```


#### RunPipeline

```bash
mkdir build
cd build
cmake ..
make RunPipeline
```

#### EdgeCaffe Library

```bash
mkdir build
cd build
cmake ..
make EdgeCaffeCore
```

#### PyEdgeCaffe

```
mkdir build
cd build
cmake ..
make py_edge_caffe
```

#### Modelsplitter

```bash
mkdir build
cd build
cmake ..
make ModelSplitter
```

## Run

### C++

The targets can be build with `Cmake`. There 3 binary examples in this project:

* **RunPipeline**: The main executable to run and profile DNNs.
  * Build: `make RunPipeline`
  * Usage: `./RunPipeline <mode> [outputfile.csv]`. The mode can be one of the scheduling policies: `linear`, `bulk`, `deepeye` or `partial`. The output file argument is optional and can be set to define the name of the output file used to write the profiling data towards. The default value of the output file is `output.csv` in the analysis folder.
* **Modelsplitter**: A tool used to split caffemodel files in smaller model files.
  * Build: `make ModelSplitter`
  * Usage: `./ModelSplitter pathToModel1 [pathToModel2] ...`
* **ScheduledPipeline**: Provides almost the same functionality as **RunPipeline** but the implementation is more exposed. 
  * Build: `make ScheduledPipeline`
  * Usage: `./ScheduledPipeline`

### Python

Add python directory (of the EdgeCaffe project) to python path

```bash
export PYTHONPATH=path/to/python_dir:$PYTHONPATH
```

Import EdgeCaffe into python

```bash
$ python3
>>> import edgecaffe
```

#### Examples

The examples folder at `python/examples` holds some basic python example scripts.

## Download models

The model files of the networks can be downloaded using the following link: https://bartcox.stackstorage.com/s/GmO0bKJb4JV5Qvd

The model files needs to be placed in the networks folder in their respective network folder. For example: the file `age_net.caffemodel` needs to be placed as `networks/AgeNet/age_net.caffemodel`.

After placing the models, run `cmake ..` in the build folder to copy the model resources to the build directory.

## Contribute

The following work-flow is used for contributions:

* Clone repository
* Checkout `master` branch: `git checkout master`
* Create a new branch to work in: `git checkout -b <branch-name>`
* Create a Merge Request to merge to master when done.