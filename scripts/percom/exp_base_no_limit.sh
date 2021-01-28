#!/usr/bin/env bash

# Arguments: BYTE_LIMIT, build_folder, CMD_str

BYTE_LIMIT=$1
BUILD_FOLDER=$2
CMD=$3
N_THREADS=$4
CUR_WD=$(eval pwd)
export OPENBLAS_NUM_THREADS=$N_THREADS
cd ./$BUILD_FOLDER
pwd
$CMD
cd $CUR_WD
