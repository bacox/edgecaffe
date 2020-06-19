#!/usr/bin/env bash

# Arguments: BYTE_LIMIT, build_folder, CMD_str

BYTE_LIMIT=$1
BUILD_FOLDER=$2
CMD=$3
CUR_WD=$(eval pwd)
export OPENBLAS_NUM_THREADS=2
cd ./$BUILD_FOLDER
$CMD
cd $CUR_WD