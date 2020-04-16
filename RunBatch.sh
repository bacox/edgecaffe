#!/usr/bin/env bash
cd ./cmake-build-debug
CMD=./RunPipeline
for i in {1..17}   # you can also use {0..9}
do
    FILENAME="SoS-linear-${i}.csv"
#  echo $FILENAME
    $CMD 'linear' $FILENAME
done