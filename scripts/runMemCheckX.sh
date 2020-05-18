#!/usr/bin/env bash
cd ./cmake-build-debug
CMD='./RunPipeline'
for i in {1..10}   # you can also use {0..9}
do
#  echo $FILENAME
#    pwd
#    ls
    $CMD 'linear'
    mv allocations.csv allocation${i}.csv
done