#!/bin/bash
DEVICE='rpi-1'
python3 scripts/percom/queue_runner.py config2queue --device=$DEVICE --exp-type=batch --file percom-$DEVICE.tmp.txt
python3 scripts/percom/queue_runner.py config2queue --device=$DEVICE --exp-type=stochastic --file percom-$DEVICE.tmp.txt --append

DEVICE='rpi-2'
python3 scripts/percom/queue_runner.py config2queue --device=$DEVICE --exp-type=batch --file percom-$DEVICE.tmp.txt
python3 scripts/percom/queue_runner.py config2queue --device=$DEVICE --exp-type=stochastic --file percom-$DEVICE.tmp.txt --append

DEVICE='rpi-4'
python3 scripts/percom/queue_runner.py config2queue --device=$DEVICE --exp-type=batch --file percom-$DEVICE.tmp.txt
python3 scripts/percom/queue_runner.py config2queue --device=$DEVICE --exp-type=stochastic --file percom-$DEVICE.tmp.txt --append
