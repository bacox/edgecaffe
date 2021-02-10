#!/bin/bash

FILE=example.tmp.txt
echo "" > $FILE
echo "bulk_example experiments/example/configs/2G/bulk-example.yaml" >> $FILE
echo "deepeye_example experiments/example/configs/2G/deepeye_example.yaml" >> $FILE
echo "masa_example experiments/example/configs/2G/masa-example.yaml" >> $FILE