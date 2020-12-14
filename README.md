# EdgeCaffe (Ubuntu 18.04)

The EdgeCaffe project aims to run the [Caffe Deep Learning framework](https://github.com/BVLC/caffe) on edge devices. The code has is targeted for Ubuntu 18.04 for both x86-64 and Raspberry Pi machines (ARMv8-A).

The main codebase is written in C++, although there are limited bindings to Python.

Additional documentation can be found at the [wiki](https://gitlab.com/bacox/edgecaffe/-/wikis/home).

## Project organization

The project is divided in six sections:

* `./caffe`: This holds the original caffe code.
* `./Core`: The code extension to facilitate the partial loading and execution.
* `./networks`: The networks and their descriptive files. Each model to work needs 3 files:
  * `description.yml` file that describes the network for EdgeCaffe
  * `.prototxt` file that describes the network architecture
  * `.caffemodel` file that holds all the trained parameters
* `./tools`: Code for the tools: `ModelSplitter`, `ExtendNetworkDescription`, `NetworkDependencyGraph`
* `./analysis`: This folder is empty on initialization of the project but will hold the output data of executable `RunPipeline`.
* `./resources`: Holds the test data files.

## Install dependencies

Cmake (VERSION  >= 3.13.0)

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
sudo apt-get install -y libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev libboost-all-dev protobuf-compiler
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
#### ExtendNetworkDescription

```bash
mkdir build
cd build
cmake ..
make ExtendNetworkDescription
```

## Prepare before running
### Download models

The model files of the networks can be downloaded using the following link: https://bartcox.stackstorage.com/s/GmO0bKJb4JV5Qvd

The model files needs to be placed in the networks folder in their respective network folder. For example: the file `age_net.caffemodel` needs to be placed as `networks/AgeNet/age_net.caffemodel`.

After placing the models, run `cmake ..` in the build folder to copy the model resources to the build directory.

### Split the models
Use the modelsplitter to split the models in smaller (partial) model files.
For example
```bash
./ModelSplitter ../networks/AgeNet/age_net.caffemodel ../networks/GenderNet/gender_net.caffemodel ../networks/FaceNet/face_full_conv.caffemodel ../networks/SoS/AlexNet_SalObjSub.caffemodel ../networks/SoS_GoogleNet/GoogleNet_SOS.caffemodel
```
### Generate model statistics (optional)
The python notebook `layer-satistics.exp.ipynb` in the `analysis` folder can be used to generate estimated loading and execution times for a network.
The generated files can be placed in the folder of a network (where `description.yaml` is located) to extend its description.

### Extend the network descriptions (optional)
This is needed when running time based scheduling policies such as `SJF` (Shortest Job First). Additional information about scheduling can found in the wiki [here](https://gitlab.com/bacox/edgecaffe/-/wikis/Scheduling).

Run the tool `ExtendNetworkDescription` to extend the networks with more accurate layer descriptions. For example 

```bash
ExtendNetworkDescription ../../networks/AgeNet ../../networks/GenderNet
```

## Run

### C++

The targets can be build with `Cmake`. There 3 binary examples in this project:

* **RunPipeline**: The main executable to run and profile DNNs. Note: It is important that the models are split (With the `ModelSplitter`) before running this executable.
  * Build: `make RunPipeline`
  * Usage: `./RunPipeline <mode> [outputfile.csv]`. The mode can be one of the scheduling policies: `linear`, `bulk`, `deepeye` or `partial`. The output file argument is optional and can be set to define the name of the output file used to write the profiling data towards. The default value of the output file is `output.csv` in the analysis folder.
* **Modelsplitter**: A tool used to split caffemodel files in smaller model files.
  * Build: `make ModelSplitter`
  * Usage: `./ModelSplitter pathToModel1 [pathToModel2] ...`
  * Example: `./ModelSplitter ../networks/AgeNet/age_net.caffemodel ../networks/GenderNet/gender_net.caffemodel`
* **ExtendNetworkDescription**: A tool used to extend the layer descriptions of the network description files with more accurate information.
  * Build: `make ExtendNetworkDescription`
  * Usage: `./ExtendNetworkDescription pathToNetworkDir [pathToNetworkDir2] ...`
  * Example: `ExtendNetworkDescription ../../networks/AgeNet ../../networks/GenderNet`
* **ScheduledPipeline**: Provides almost the same functionality as **RunPipeline** but the implementation is more exposed. Note: It is important that the models are split (With the `ModelSplitter`) before running this executable. 
  * Build: `make ScheduledPipeline`
  * Usage: `./ScheduledPipeline`
* **Exp_const_arrivals**: Almost same example as `RunPipeline` but it uses a distribution of arrivals instead of submitting everything at the beginning instantly. Note: It is important that the models are split (With the `ModelSplitter`) before running this executable.
  * Build: `make Exp_const_arrivals`
  * Help: `./Exp_const_arrivals --help`
```
Usage:
  Exp_const_arrivals [OPTION...]

  -m, --mode arg            Mode to split and run the networks. Values
                            [partial|linear|deepeye|bulk]
      --mem_limit arg       The memory limit given by the OS to EdgeCaffe.
                            NOTE: this does not limit the memory for this
                            process but is used in output generation and in
                            scheduling.
      --seed arg            Seed for random number generator
  -V, --verbose             Verbose
  -N, --num-arrivals arg    Number of arrivals to be generated
  -a, --arrival-list arg    NOT_YET_IMPLEMENTED. Use this arrival list to
                            inject arrivals instead of the generated one
  -p, --output-prefix arg   Prefix for all output files to make it them
                            unique for a certain run
      --output-path arg     Define the path to store all output files
      --network-path arg    Define the path to store all output files
      --resources-path arg  Define the path to store all output files
  -s, --sched-alg arg       The scheduling algorithm to be used: [FCFS|SJF]
  -c, --read-config arg     Use a yaml config file to configure this run
                            instead of the cli. This will overrule all other
                            arguments. Example 'config/pipeline-template.yaml'
  -h, --help                Print help message
```
* **NetworkDependencyGraph**: A tool used to generate dependency graphs for all the modes (partial|linear|bulk|deepeye) for the specified network. The NetworkDependencyGraph tool will create `.dot` files that can be used to generate a png or another image format. The script `dot2png.sh` can be used to automate the conversion of dot files to images. The use of the `dot` command might require `sudo apt-get install graphviz`. Example of `.dot` files conversion: `dot -Tpng AgeNet0-partial.dot -o AgeNet0-partial.png`. For more informatio about dot and graphviz see [https://renenyffenegger.ch/notes/tools/Graphviz/examples/index](https://renenyffenegger.ch/notes/tools/Graphviz/examples/index)
  * Build: `make ndg`
  * Usage: `./ndg <networkName> <pathToNetwork>`
  * Example: `./ndg AgeNet ../networks/AgeNet`

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

### Examples

The examples folder at `python/examples` holds some basic python example scripts.

## Memory swapping
Memory swapping is an important feature to enable to prevent EdgeCaffe from being killed when is uses too much memory.
A more deailted description can be found [here](https://gitlab.com/bacox/edgecaffe/-/wikis/Memory-Swapping)
### Create swap file

If necessary, you can increase the size of the swap file. To check the current size of the swap file `grep SwapTotal /proc/meminfo`.

* Turn off swapping `sudo swapoff -a`
* Resize the swap file `sudo dd if=/dev/zero of=/swapfile bs=1G count=8`. This example creates a file of 8GB.
* Mark file as swapfile `sudo mkswap /swapfile`
* Activate the swapfile `sudo swapon /swapfile`
* Enable swapping `sudo swapon -a`

### Run process with limited memory

To constrain the process in its resources we create a new cgroup and set the memory limits for both the virtual memory and the swap memory. 

```bash
# Create new cgroup
sudo cgcreate -g memory:force-swap
# Set virtual memory limit to 2 GB
sudo cgset -r memory.limit_in_bytes=2G force-swap
# Set allowed swapped memory to 8GB
sudo cgset -r memory.memsw.limit_in_bytes=8G force-swap
# Show the configuration
sudo cgget -g memory:force-swap | grep bytes

# Run procress within cgroup
sudo cgexec -g memory:force-swap <program> <program arguments>

# Delete group if needed
sudo cgdelete -g memory:force-swap
```
## Analysis
The folder `analysis` hold two python notebooks that can be used for analysis.

* `profile-networks.exp.ipynb`: The notebook can be used to generate basic profiling information for each network. The loading and execution time of the layers are plotted, and the effects of running under memory constraints are shown.
* `layer-satistics.exp.ipynb`: This notebook can be used to generate the statistic files for each network. It calculates the mean of the loading and execution times of each layer in each network and outputs this to a csv file.

## Contribute

The following work-flow is used for contributions:

* Clone repository
* Checkout `master` branch: `git checkout master`
* Create a new branch to work in: `git checkout -b <branch-name>`
* Create a Merge Request to merge to master when done.