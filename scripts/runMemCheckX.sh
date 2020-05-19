#!/usr/bin/env bash
##
# To use this script make sure -DMEMORY_CHECK_ON is enabled in CMAKE
# Alter the first line to the build folder if required.
##

cd ./cmake-build-release
CMD='./RunPipeline'
TARGET_PATH='../analysis'
for i in {1..10}   # you can also use {0..9}
do
    $CMD 'linear'
    mv ${TARGET_PATH}/allocations.csv ${TARGET_PATH}/allocations${i}.csv
done