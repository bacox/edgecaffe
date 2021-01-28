#!/usr/bin/env bash

# Arguments: BYTE_LIMIT, build_folder, CMD_str, number of blas threads

BYTE_LIMIT=$1
BUILD_FOLDER=$2
CMD=$3
N_THREADS=$4
CUR_WD=$(eval pwd)
export OPENBLAS_NUM_THREADS=$N_THREADS
cd ./$BUILD_FOLDER
sudo cgcreate -g memory:bulk-inference
sudo cgset -r memory.limit_in_bytes="${BYTE_LIMIT}" bulk-inference
sudo cgset -r memory.memsw.limit_in_bytes=12G bulk-inference
sudo cgexec -g memory:bulk-inference $CMD
sudo cgdelete -g memory:bulk-inference
cd $CUR_WD