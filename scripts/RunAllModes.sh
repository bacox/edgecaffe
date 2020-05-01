#!/usr/bin/env bash
HOST=`hostname`

export OPENBLAS_NUM_THREADS=2
for i in {1..17}   # you can also use {0..9}
do
    for modeName in partial bulk deepeye linear; do
        CMD="./RunPipeline ${modeName} output_${HOST}_${modeName}_${i}.csv"
        $CMD
    done
done
