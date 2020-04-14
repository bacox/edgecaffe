#!/usr/bin/env bash

#BYTE_LIMIT=$1
#MODE=$2
cd cmake-build-debug-openblas
export OPENBLAS_NUM_THREADS=2


for BYTE_LIMIT in 4G 2G 1G 512M 256M 128M; do

    echo "*****************************"
    echo "Running with ${BYTE_LIMIT} LIMIT"
    echo "*****************************"

    sudo cgcreate -g memory:bulk-inference
    sudo cgset -r memory.limit_in_bytes="${BYTE_LIMIT}" bulk-inference
    sudo cgset -r memory.memsw.limit_in_bytes=8G bulk-inference
#    sudo cgget -g memory:bulk-inference | grep bytes

    for modeName in partial bulk deepeye linear; do
      CMD="./RunPipeline ${modeName} output_${modeName}_${BYTE_LIMIT}.csv"
      sudo cgexec -g memory:bulk-inference $CMD
    done

    sudo cgdelete -g memory:bulk-inference
done

#sudo cgcreate -g memory:bulk-inference
#sudo cgset -r memory.limit_in_bytes="${BYTE_LIMIT}" bulk-inference
#sudo cgset -r memory.memsw.limit_in_bytes=8G bulk-inference
#sudo cgget -g memory:bulk-inference | grep bytes
#
##MODE='linear'
#
#
#for modeName in partial bulk deepeye linear; do
#  CMD="./RunPipeline ${modeName} output_${modeName}_${BYTE_LIMIT}.csv"
#  sudo cgexec -g memory:bulk-inference $CMD
#done
#
##echo $CMD
#sudo cgdelete -g memory:bulk-inference
