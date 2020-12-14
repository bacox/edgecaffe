# Scripts
The scripts folder contains scripts to automate running and testing.

For al the stochastic experiments, the scripts in the folder `poisson_exp` are used.
* `RunConstrained.sh`: Configure the memory limitation in a cgroup and then execute the pipeline in the cgroup.
* `RunBatch.sh`: Run the pipeline a certain number of replications.
* `RunAllModes.sh`: Run all the current known policies (partial, bulk, deepeye, linear) and repeat this a X number of times.
* `default_modelsplitter.sh`: Shortcut to run the ModelSplitter on the trained networks. The default location of the ModelSplitter binary in this script is the current directory.

Raspberry pi's are used in some situations to test on target hardware. The following scripts are **not** used when running the system on a laptop.
* `close_slaves.sh`: Shutdown all the raspberry pi's under the aliases `rpi_1`, `rpi_2`, and `rpi_4`
* `retrieve_slave_output.sh`: Collect the (csv) output files from the raspberry pi's
* `copy_models.sh`: Copy the binary files of the trained models to the raspberry pi's
* `distribute_deb.sh`: Distribute the build debian package to the raspberry pi's
* `copy_repo.sh`: Copy the repository files to remote alias `rpi_4`
* `init_rpi.sh`: Initialize the raspberry pi (and install deps and copy resources)
* `copy_resources.sh`: Copy the resources file to the raspberry pi's
* `install_rpi.sh`: Install all dependencies for Ubuntu Server 18.04 ARM64

## poisson_exp
* `baseline.sh`: Deprecated
* `poisson_rv_nf_base.py`: Deprecated
* `poisson_rv_nf_base.sh`: shell script used to set the c-group with the correct memory limitations, and blas threads
* `poisson_rv_nf_multi.py`: Main script to run multiple (poisson) experiments 
* `poisson_rv_nf_rerun.py`: Deprecated
* `poisson_rv_nf_rerun.sh`: Deprecated

### poisson_exp/rpi
Contains scripts for running on the raspberry pi
`poisson_rv_nf_base_limit.sh`: shell script used to set the c-group with the correct memory limitations, and blas threads
`poisson_rv_nf_base.sh`: shell script used to set nothing but the number of blas threads
